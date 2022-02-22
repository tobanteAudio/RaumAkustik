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

MainComponent::MainComponent()
{
    addAndMakeVisible(_load);
    addAndMakeVisible(_save);
    addAndMakeVisible(_roomProperties);

    _load.onClick = [this] { load(); };
    _save.onClick = [this] { save(); };

    connectValuesToTree();
    setSize(600, 400);
}

void MainComponent::paint(juce::Graphics& g)
{
    auto const room = RoomLayout {
        RoomDimensions {
            static_cast<double>(_roomLength.getValue()),
            static_cast<double>(_roomWidth.getValue()),
            static_cast<double>(_roomHeight.getValue()),
        },
        Point {
            static_cast<double>(_leftX.getValue()),
            static_cast<double>(_leftY.getValue()),
            static_cast<double>(_leftZ.getValue()),
        },
        Point {
            static_cast<double>(_rightX.getValue()),
            static_cast<double>(_rightY.getValue()),
            static_cast<double>(_rightZ.getValue()),
        },
        Point {
            static_cast<double>(_listenX.getValue()),
            static_cast<double>(_listenY.getValue()),
            static_cast<double>(_listenZ.getValue()),
        },
    };

    if (room.dimensions.length == 0.0) { return; }
    if (room.dimensions.width == 0.0) { return; }
    if (room.dimensions.height == 0.0) { return; }

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    auto const area            = _drawArea.toDouble();
    auto const scaleFactor     = room.dimensions.length / (area.getHeight() * 0.9);
    auto const roomLengthPixel = room.dimensions.length / scaleFactor;
    auto const roomWidthPixel  = room.dimensions.width / scaleFactor;
    auto const roomArea        = juce::Rectangle {area.getX(), area.getY(), roomWidthPixel, roomLengthPixel}.withCentre(
               _drawArea.getCentre().toDouble());

    g.setColour(juce::Colours::black);
    g.drawRect(roomArea.toFloat(), 2.0f);

    auto const iconSize = static_cast<double>(_iconSize.getValue());

    auto const leftX    = roomArea.getX() + room.leftSpeaker.x / scaleFactor;
    auto const leftY    = roomArea.getY() + room.leftSpeaker.y / scaleFactor;
    auto const leftArea = juce::Rectangle {0.0, 0.0, iconSize, iconSize}.withCentre({leftX, leftY});
    _speakerIcon->drawWithin(g, leftArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    auto const rightX    = roomArea.getX() + room.rightSpeaker.x / scaleFactor;
    auto const rightY    = roomArea.getY() + room.rightSpeaker.y / scaleFactor;
    auto const rightArea = juce::Rectangle {0.0, 0.0, iconSize, iconSize}.withCentre({rightX, rightY});
    _speakerIcon->drawWithin(g, rightArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    auto const listenX    = roomArea.getX() + room.listenPosition.x / scaleFactor;
    auto const listenY    = roomArea.getY() + room.listenPosition.y / scaleFactor;
    auto const listenArea = juce::Rectangle {0.0, 0.0, iconSize, iconSize}.withCentre({listenX, listenY});
    _headIcon->drawWithin(g, listenArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    auto const reflectionLeftClose = reflectionLeftSpeaker(room);
    auto const toWall = juce::Line {leftX, leftY, roomArea.getX(), roomArea.getY() + reflectionLeftClose / scaleFactor};
    auto const toListen
        = juce::Line {roomArea.getX(), roomArea.getY() + reflectionLeftClose / scaleFactor, listenX, listenY};
    g.drawLine(toWall.toFloat());
    g.drawLine(toListen.toFloat());

    // DBG("Reflection (right): " + juce::String {reflectionRightSpeaker(room)});
    // DBG("Reflection Far (left): " + juce::String {reflectionLeftSpeakerFar(room)});
    // DBG("Reflection Far (right): " + juce::String {reflectionRightSpeakerFar(room)});
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    _drawArea = area.removeFromRight(area.proportionOfWidth(0.5)).reduced(10);

    area.reduce(10, 10);
    auto buttonArea = area.removeFromTop(area.proportionOfHeight(0.1));
    _load.setBounds(buttonArea.removeFromLeft(buttonArea.proportionOfWidth(0.5)).reduced(5));
    _save.setBounds(buttonArea.reduced(5));

    _roomProperties.setBounds(area);
}

auto MainComponent::connectValuesToTree() -> void
{
    _iconSize.removeListener(this);

    _roomLength.removeListener(this);
    _roomWidth.removeListener(this);
    _roomHeight.removeListener(this);

    _listenX.removeListener(this);
    _listenY.removeListener(this);
    _listenZ.removeListener(this);

    _leftX.removeListener(this);
    _leftY.removeListener(this);
    _leftZ.removeListener(this);

    _rightX.removeListener(this);
    _rightY.removeListener(this);
    _rightZ.removeListener(this);

    _iconSize.referTo(_roomTree.getPropertyAsValue("icon_size", &_undoManager));

    _roomLength.referTo(_roomTree.getPropertyAsValue("room_length", &_undoManager));
    _roomWidth.referTo(_roomTree.getPropertyAsValue("room_width", &_undoManager));
    _roomHeight.referTo(_roomTree.getPropertyAsValue("room_height", &_undoManager));

    _listenX.referTo(_roomTree.getPropertyAsValue("listen_x", &_undoManager));
    _listenY.referTo(_roomTree.getPropertyAsValue("listen_y", &_undoManager));
    _listenZ.referTo(_roomTree.getPropertyAsValue("listen_z", &_undoManager));

    _leftX.referTo(_roomTree.getPropertyAsValue("left_x", &_undoManager));
    _leftY.referTo(_roomTree.getPropertyAsValue("left_y", &_undoManager));
    _leftZ.referTo(_roomTree.getPropertyAsValue("left_z", &_undoManager));

    _rightX.referTo(_roomTree.getPropertyAsValue("right_x", &_undoManager));
    _rightY.referTo(_roomTree.getPropertyAsValue("right_y", &_undoManager));
    _rightZ.referTo(_roomTree.getPropertyAsValue("right_z", &_undoManager));

    _iconSize.addListener(this);

    _roomLength.addListener(this);
    _roomWidth.addListener(this);
    _roomHeight.addListener(this);

    _listenX.addListener(this);
    _listenY.addListener(this);
    _listenZ.addListener(this);

    _leftX.addListener(this);
    _leftY.addListener(this);
    _leftZ.addListener(this);

    _rightX.addListener(this);
    _rightY.addListener(this);
    _rightZ.addListener(this);

    _roomProperties.clear();

    _roomProperties.addSection("General",
                               juce::Array<juce::PropertyComponent*> {
                                   new juce::SliderPropertyComponent {_iconSize, "Icon Size", 20.0, 100.0, 1.0},
                               });

    _roomProperties.addSection("Room Dimensions",
                               juce::Array<juce::PropertyComponent*> {
                                   new juce::SliderPropertyComponent {_roomLength, "Length", 0.0, 1000.0, 1.0},
                                   new juce::SliderPropertyComponent {_roomWidth, "Width", 0.0, 1000.0, 1.0},
                                   new juce::SliderPropertyComponent {_roomHeight, "Height", 0.0, 1000.0, 1.0},
                               });

    _roomProperties.addSection("Listen Position",
                               juce::Array<juce::PropertyComponent*> {
                                   new juce::SliderPropertyComponent {_listenX, "X", 0.0, 1000.0, 1.0},
                                   new juce::SliderPropertyComponent {_listenY, "Y", 0.0, 1000.0, 1.0},
                                   new juce::SliderPropertyComponent {_listenZ, "Z", 0.0, 1000.0, 1.0},
                               });

    _roomProperties.addSection("Left Speaker", juce::Array<juce::PropertyComponent*> {
                                                   new juce::SliderPropertyComponent {_leftX, "X", 0.0, 1000.0, 1.0},
                                                   new juce::SliderPropertyComponent {_leftY, "Y", 0.0, 1000.0, 1.0},
                                                   new juce::SliderPropertyComponent {_leftZ, "Z", 0.0, 1000.0, 1.0},
                                               });

    _roomProperties.addSection("Right Speaker", juce::Array<juce::PropertyComponent*> {
                                                    new juce::SliderPropertyComponent {_rightX, "X", 0.0, 1000.0, 1.0},
                                                    new juce::SliderPropertyComponent {_rightY, "Y", 0.0, 1000.0, 1.0},
                                                    new juce::SliderPropertyComponent {_rightZ, "Z", 0.0, 1000.0, 1.0},
                                                });
}

auto MainComponent::save() -> void
{
    auto file   = juce::File {juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    auto stream = juce::FileOutputStream {file};
    _roomTree.writeToStream(stream);
}

auto MainComponent::load() -> void
{
    auto file   = juce::File {juce::File::getSpecialLocation(juce::File::userHomeDirectory)}.getChildFile("test.mcra");
    auto stream = juce::FileInputStream {file};
    _roomTree   = juce::ValueTree::readFromStream(stream);
    connectValuesToTree();
    repaint();
}

auto MainComponent::valueChanged(juce::Value& /*value*/) -> void { repaint(); }