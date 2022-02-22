#include "MainComponent.hpp"

struct Point
{
    double x {0};
    double y {0};
    double z {0};
};

struct RoomDimensions
{
    double length {0};
    double width {0};
    double height {0};
};

struct RoomLayout
{
    RoomDimensions dimensions {};
    Point leftSpeaker {};
    Point rightSpeaker {};
    Point listenPosition {};
};

auto reflectionLeftSpeaker(RoomLayout const& room) -> double
{
    auto const y   = room.listenPosition.y - room.leftSpeaker.y;
    auto const x1  = room.leftSpeaker.x;
    auto const x2  = room.dimensions.width - room.listenPosition.x;
    auto const tmp = (y * x2) / (x1 + x2);
    return room.listenPosition.y - tmp;
};

auto reflectionLeftSpeakerFar(RoomLayout const& room) -> double
{
    auto const y   = room.listenPosition.y - room.leftSpeaker.y;
    auto const x1  = room.dimensions.width - room.leftSpeaker.x;
    auto const x2  = room.dimensions.width - room.listenPosition.x;
    auto const tmp = (y * x2) / (x1 + x2);
    return room.listenPosition.y - tmp;
};

auto reflectionRightSpeaker(RoomLayout const& room) -> double
{
    auto const y   = room.listenPosition.y - room.rightSpeaker.y;
    auto const x1  = room.dimensions.width - room.rightSpeaker.x;
    auto const x2  = room.dimensions.width - room.listenPosition.x;
    auto const tmp = (y * x2) / (x1 + x2);
    return room.listenPosition.y - tmp;
};

auto reflectionRightSpeakerFar(RoomLayout const& room) -> double
{
    auto const y   = room.listenPosition.y - room.rightSpeaker.y;
    auto const x1  = room.rightSpeaker.x;
    auto const x2  = room.dimensions.width - room.listenPosition.x;
    auto const tmp = (y * x2) / (x1 + x2);
    return room.listenPosition.y - tmp;
};

static constexpr auto room = RoomLayout {
    RoomDimensions {
        600,
        365,
        312,
    },
    Point {
        121,
        160,
        125,
    },
    Point {
        121 + 123,
        160,
        125,
    },
    Point {
        121 + 123 / 2.0,
        160 + 123,
        120,
    },
};

MainComponent::MainComponent()
{
    addAndMakeVisible(_roomLengthSlider);
    addAndMakeVisible(_roomWidthSlider);
    addAndMakeVisible(_roomHeightSlider);

    _roomLengthSlider.setRange({0.0, 1000.0}, 1.0);
    _roomWidthSlider.setRange({0.0, 1000.0}, 1.0);
    _roomHeightSlider.setRange({0.0, 1000.0}, 1.0);

    _roomLengthSlider.setValue(600.0, juce::dontSendNotification);
    _roomWidthSlider.setValue(365.0, juce::dontSendNotification);
    _roomHeightSlider.setValue(312.0, juce::dontSendNotification);

    _roomLengthSlider.onValueChange = [this] { repaint(); };
    _roomWidthSlider.onValueChange  = [this] { repaint(); };
    _roomHeightSlider.onValueChange = [this] { repaint(); };

    setSize(600, 400);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    auto const area            = _drawArea.toDouble();
    auto const scaleFactor     = static_cast<double>(_roomLength.getValue()) / (area.getHeight() * 0.9);
    auto const roomLengthPixel = static_cast<double>(_roomLength.getValue()) / scaleFactor;
    auto const roomWidthPixel  = static_cast<double>(_roomWidth.getValue()) / scaleFactor;
    auto const roomArea        = juce::Rectangle {area.getX(), area.getY(), roomWidthPixel, roomLengthPixel}.withCentre(
               _drawArea.getCentre().toDouble());

    g.setColour(juce::Colours::black);
    g.drawRect(roomArea.toFloat(), 2.0f);

    auto const leftX    = roomArea.getX() + room.leftSpeaker.x / scaleFactor;
    auto const leftY    = roomArea.getY() + room.leftSpeaker.y / scaleFactor;
    auto const leftArea = juce::Rectangle {0.0, 0.0, 20.0, 20.0}.withCentre({leftX, leftY});
    g.fillRect(leftArea.toFloat());

    auto const rightX    = roomArea.getX() + room.rightSpeaker.x / scaleFactor;
    auto const rightY    = roomArea.getY() + room.rightSpeaker.y / scaleFactor;
    auto const rightArea = juce::Rectangle {0.0, 0.0, 20.0, 20.0}.withCentre({rightX, rightY});
    g.fillRect(rightArea.toFloat());

    auto const listenX    = roomArea.getX() + room.listenPosition.x / scaleFactor;
    auto const listenY    = roomArea.getY() + room.listenPosition.y / scaleFactor;
    auto const listenArea = juce::Rectangle {0.0, 0.0, 20.0, 20.0}.withCentre({listenX, listenY});
    g.fillRect(listenArea.toFloat());

    auto const reflectionLeftClose = reflectionLeftSpeaker(room);
    g.drawLine(leftX, leftY, roomArea.getX(), roomArea.getY() + reflectionLeftClose / scaleFactor);
    g.drawLine(roomArea.getX(), roomArea.getY() + reflectionLeftClose / scaleFactor, listenX, listenY);
    DBG("Reflection (right): " + juce::String {reflectionRightSpeaker(room)});

    DBG("Reflection Far (left): " + juce::String {reflectionLeftSpeakerFar(room)});
    DBG("Reflection Far (right): " + juce::String {reflectionRightSpeakerFar(room)});
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    _drawArea = area.removeFromRight(area.proportionOfWidth(0.5)).reduced(10);

    area                    = area.reduced(10);
    auto const sliderHeight = area.getHeight() / 3;
    _roomLengthSlider.setBounds(area.removeFromTop(sliderHeight).reduced(5));
    _roomWidthSlider.setBounds(area.removeFromTop(sliderHeight).reduced(5));
    _roomHeightSlider.setBounds(area.removeFromTop(sliderHeight).reduced(5));
}
