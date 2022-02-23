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
    addAndMakeVisible(_undo);
    addAndMakeVisible(_redo);
    addAndMakeVisible(_load);
    addAndMakeVisible(_save);
    addAndMakeVisible(_roomProperties);
    addAndMakeVisible(_renderProperties);

    _undo.onClick = [this] { _undoManager.undo(); };
    _redo.onClick = [this] { _undoManager.redo(); };
    _save.onClick = [this] { save(); };
    _load.onClick = [this] { load(); };

    _undo.setImages(juce::Drawable::createFromImageData(mcbd::undo_svg, mcbd::undo_svgSize).get());
    _redo.setImages(juce::Drawable::createFromImageData(mcbd::redo_svg, mcbd::redo_svgSize).get());
    _save.setImages(juce::Drawable::createFromImageData(mcbd::save_svg, mcbd::save_svgSize).get());
    _load.setImages(juce::Drawable::createFromImageData(mcbd::folder_open_svg, mcbd::folder_open_svgSize).get());

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

    auto const iconSize = std::max(static_cast<double>(_iconSize.getValue()), 1.0);
    auto const iconRect = juce::Rectangle {0.0, 0.0, iconSize, iconSize};

    if (room.dimensions.length == 0.0) { return; }
    if (room.dimensions.width == 0.0) { return; }
    if (room.dimensions.height == 0.0) { return; }

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    auto totalArea          = _drawArea;
    auto const topViewArea  = totalArea.removeFromTop(totalArea.proportionOfHeight(0.66));
    auto const area         = topViewArea.toDouble();
    auto const scaleFactor  = room.dimensions.length / (area.getHeight() * 0.9);
    auto const roomLengthPx = room.dimensions.length / scaleFactor;
    auto const roomWidthPx  = room.dimensions.width / scaleFactor;
    auto const topViewRoom  = juce::Rectangle {area.getX(), area.getY(), roomWidthPx, roomLengthPx}.withCentre(
         topViewArea.getCentre().toDouble());

    g.setColour(juce::Colours::black);
    g.drawRect(topViewRoom.toFloat(), 8.0f);

    auto const leftX    = topViewRoom.getX() + room.leftSpeaker.x / scaleFactor;
    auto const leftY    = topViewRoom.getY() + room.leftSpeaker.y / scaleFactor;
    auto const leftArea = iconRect.withCentre({leftX, leftY});
    _speakerIcon->drawWithin(g, leftArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    auto const rightX    = topViewRoom.getX() + room.rightSpeaker.x / scaleFactor;
    auto const rightY    = topViewRoom.getY() + room.rightSpeaker.y / scaleFactor;
    auto const rightArea = iconRect.withCentre({rightX, rightY});
    _speakerIcon->drawWithin(g, rightArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    auto const listenX    = topViewRoom.getX() + room.listenPosition.x / scaleFactor;
    auto const listenY    = topViewRoom.getY() + room.listenPosition.y / scaleFactor;
    auto const listenArea = iconRect.withCentre({listenX, listenY});
    _headIcon->drawWithin(g, listenArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    if (static_cast<bool>(_renderLeftReflections.getValue()))
    {
        auto const reflectionLeftClose = reflectionLeftSpeaker(room);
        auto const leftCloseToWall
            = juce::Line {leftX, leftY, topViewRoom.getX(), topViewRoom.getY() + reflectionLeftClose / scaleFactor};
        auto const leftCloseToListen
            = juce::Line {topViewRoom.getX(), topViewRoom.getY() + reflectionLeftClose / scaleFactor, listenX, listenY};
        g.drawLine(leftCloseToWall.toFloat());
        g.drawLine(leftCloseToListen.toFloat());

        auto const reflectionLeftFar = reflectionLeftSpeakerFar(room);
        auto const leftFarToWall
            = juce::Line {leftX, leftY, topViewRoom.getRight(), topViewRoom.getY() + reflectionLeftFar / scaleFactor};
        auto const leftFarToListen = juce::Line {
            topViewRoom.getRight(), topViewRoom.getY() + reflectionLeftFar / scaleFactor, listenX, listenY};
        g.drawLine(leftFarToWall.toFloat());
        g.drawLine(leftFarToListen.toFloat());
    }

    ///////////////////////////////////////// FRONT
    auto const frontArea    = totalArea.toDouble();
    auto const roomHeightPx = room.dimensions.height / scaleFactor;
    auto const frontView    = juce::Rectangle {0.0, 0.0, roomWidthPx, roomHeightPx}.withCentre(frontArea.getCentre());
    g.drawRect(frontView.toFloat(), 8.0f);

    auto const leftZ         = frontView.getBottom() - room.leftSpeaker.z / scaleFactor;
    auto const frontLeftArea = iconRect.withCentre({leftX, leftZ});
    _speakerIcon->drawWithin(g, frontLeftArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    auto const rightZ         = frontView.getBottom() - room.rightSpeaker.z / scaleFactor;
    auto const frontRightArea = iconRect.withCentre({rightX, rightZ});
    _speakerIcon->drawWithin(g, frontRightArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    auto const listenZ         = frontView.getBottom() - room.listenPosition.z / scaleFactor;
    auto const frontListenArea = iconRect.withCentre({listenX, listenZ});
    _headIcon->drawWithin(g, frontListenArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);

    // DBG("Reflection (right): " + juce::String {reflectionRightSpeaker(room)});
    // DBG("Reflection Far (left): " + juce::String {reflectionLeftSpeakerFar(room)});
    // DBG("Reflection Far (right): " + juce::String {reflectionRightSpeakerFar(room)});
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    _drawArea = area.removeFromRight(area.proportionOfWidth(0.5)).reduced(10);

    area.reduce(10, 10);
    auto buttonArea        = area.removeFromTop(area.proportionOfHeight(0.1));
    auto const buttonWidth = buttonArea.proportionOfWidth(0.25);
    _undo.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    _redo.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    _load.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    _save.setBounds(buttonArea.reduced(5));

    _renderProperties.setBounds(area.removeFromBottom(area.proportionOfHeight(0.2)).reduced(5));
    _roomProperties.setBounds(area.reduced(5));
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

    _renderLeftReflections.removeListener(this);
    _renderRightReflections.removeListener(this);

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

    _renderLeftReflections.referTo(_roomTree.getPropertyAsValue("render_left_reflections", &_undoManager));
    _renderRightReflections.referTo(_roomTree.getPropertyAsValue("render_right_reflections", &_undoManager));

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

    _renderLeftReflections.addListener(this);
    _renderRightReflections.addListener(this);

    _roomProperties.clear();
    _renderProperties.clear();

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

    _renderProperties.addSection("First Reflections",
                                 juce::Array<juce::PropertyComponent*> {
                                     new juce::BooleanPropertyComponent {_renderLeftReflections, "Left", "Draw"},
                                     new juce::BooleanPropertyComponent {_renderRightReflections, "Right", "Draw"},
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