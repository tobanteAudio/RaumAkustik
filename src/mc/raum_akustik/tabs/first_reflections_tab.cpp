#include "first_reflections_tab.hpp"

namespace ra
{
namespace
{
struct Point
{
    double x{0};
    double y{0};
    double z{0};
};

struct RoomDimensions
{
    double length{0};
    double width{0};
    double height{0};
};

struct RoomLayout
{
    RoomDimensions dimensions{};
    Point leftSpeaker{};
    Point rightSpeaker{};
    Point listenPosition{};
};

auto reflectionLeftSpeaker(RoomLayout const& room) -> double
{
    auto const y   = room.listenPosition.y - room.leftSpeaker.y;
    auto const x1  = room.leftSpeaker.x;
    auto const x2  = room.dimensions.width - room.listenPosition.x;
    auto const tmp = (y * x2) / (x1 + x2);
    return room.listenPosition.y - tmp;
}

auto reflectionLeftSpeakerFar(RoomLayout const& room) -> double
{
    auto const y   = room.listenPosition.y - room.leftSpeaker.y;
    auto const x1  = room.dimensions.width - room.leftSpeaker.x;
    auto const x2  = room.dimensions.width - room.listenPosition.x;
    auto const tmp = (y * x2) / (x1 + x2);
    return room.listenPosition.y - tmp;
}

[[maybe_unused]] auto reflectionRightSpeaker(RoomLayout const& room) -> double
{
    auto const y   = room.listenPosition.y - room.rightSpeaker.y;
    auto const x1  = room.dimensions.width - room.rightSpeaker.x;
    auto const x2  = room.dimensions.width - room.listenPosition.x;
    auto const tmp = (y * x2) / (x1 + x2);
    return room.listenPosition.y - tmp;
}

[[maybe_unused]] auto reflectionRightSpeakerFar(RoomLayout const& room) -> double
{
    auto const y   = room.listenPosition.y - room.rightSpeaker.y;
    auto const x1  = room.rightSpeaker.x;
    auto const x2  = room.dimensions.width - room.listenPosition.x;
    auto const tmp = (y * x2) / (x1 + x2);
    return room.listenPosition.y - tmp;
}
}  // namespace

FirstReflectionsView::FirstReflectionsView(juce::ValueTree vt, juce::UndoManager* um)
    : _undoManager{um}, _roomTree{vt.getOrCreateChildWithName("FirstReflections", um)}
{
    _iconSize.setDefault(20.0);

    addAndMakeVisible(_roomProperties);
    addAndMakeVisible(_renderProperties);

    connectValuesToTree();
    setSize(600, 400);
}

void FirstReflectionsView::paint(juce::Graphics& g)
{
    auto const room = RoomLayout{
        RoomDimensions{
            _roomLength,
            _roomWidth,
            _roomHeight,
        },
        Point{
            _leftX,
            _leftY,
            _leftZ,
        },
        Point{
            _rightX,
            _rightY,
            _rightZ,
        },
        Point{
            _listenX,
            _listenY,
            _listenZ,
        },
    };

    auto* const lnf = dynamic_cast<juce::LookAndFeel_V4*>(&getLookAndFeel());
    jassert(lnf != nullptr);

    auto const iconSize = std::max<double>(_iconSize, 1.0);
    auto const iconRect = juce::Rectangle{0.0, 0.0, iconSize, iconSize};

    if (room.dimensions.length == 0.0) { return; }
    if (room.dimensions.width == 0.0) { return; }
    if (room.dimensions.height == 0.0) { return; }

    auto totalArea = _drawArea;
    auto bgColor   = lnf->getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::widgetBackground);
    g.setColour(bgColor);
    g.fillRect(_drawArea);

    auto const topViewArea  = totalArea.removeFromTop(totalArea.proportionOfHeight(0.66));
    auto const area         = topViewArea.toDouble();
    auto const scaleFactor  = room.dimensions.length / (area.getHeight() * 0.9);
    auto const roomLengthPx = room.dimensions.length / scaleFactor;
    auto const roomWidthPx  = room.dimensions.width / scaleFactor;
    auto const topViewRoom  = juce::Rectangle{area.getX(), area.getY(), roomWidthPx, roomLengthPx}.withCentre(
        topViewArea.getCentre().toDouble());

    g.setColour(juce::Colours::black);
    g.drawRect(topViewRoom.toFloat(), 8.0F);

    auto const leftX    = topViewRoom.getX() + room.leftSpeaker.x / scaleFactor;
    auto const leftY    = topViewRoom.getY() + room.leftSpeaker.y / scaleFactor;
    auto const leftArea = iconRect.withCentre({leftX, leftY});
    _speakerIcon->drawWithin(g, leftArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    auto const rightX    = topViewRoom.getX() + room.rightSpeaker.x / scaleFactor;
    auto const rightY    = topViewRoom.getY() + room.rightSpeaker.y / scaleFactor;
    auto const rightArea = iconRect.withCentre({rightX, rightY});
    _speakerIcon->drawWithin(g, rightArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    auto const listenX    = topViewRoom.getX() + room.listenPosition.x / scaleFactor;
    auto const listenY    = topViewRoom.getY() + room.listenPosition.y / scaleFactor;
    auto const listenArea = iconRect.withCentre({listenX, listenY});
    _headIcon->drawWithin(g, listenArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    if (_renderLeftReflections)
    {
        auto const reflectionLeftClose = reflectionLeftSpeaker(room);
        auto const leftCloseToWall
            = juce::Line{leftX, leftY, topViewRoom.getX(), topViewRoom.getY() + reflectionLeftClose / scaleFactor};
        auto const leftCloseToListen
            = juce::Line{topViewRoom.getX(), topViewRoom.getY() + reflectionLeftClose / scaleFactor, listenX, listenY};
        g.drawLine(leftCloseToWall.toFloat());
        g.drawLine(leftCloseToListen.toFloat());

        auto const reflectionLeftFar = reflectionLeftSpeakerFar(room);
        auto const leftFarToWall
            = juce::Line{leftX, leftY, topViewRoom.getRight(), topViewRoom.getY() + reflectionLeftFar / scaleFactor};
        auto const leftFarToListen = juce::Line{topViewRoom.getRight(),
                                                topViewRoom.getY() + reflectionLeftFar / scaleFactor, listenX, listenY};
        g.drawLine(leftFarToWall.toFloat());
        g.drawLine(leftFarToListen.toFloat());
    }

    ///////////////////////////////////////// FRONT
    auto const frontArea    = totalArea.toDouble();
    auto const roomHeightPx = room.dimensions.height / scaleFactor;
    auto const frontView    = juce::Rectangle{0.0, 0.0, roomWidthPx, roomHeightPx}.withCentre(frontArea.getCentre());
    g.drawRect(frontView.toFloat(), 8.0F);

    auto const leftZ         = frontView.getBottom() - room.leftSpeaker.z / scaleFactor;
    auto const frontLeftArea = iconRect.withCentre({leftX, leftZ});
    _speakerIcon->drawWithin(g, frontLeftArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    auto const rightZ         = frontView.getBottom() - room.rightSpeaker.z / scaleFactor;
    auto const frontRightArea = iconRect.withCentre({rightX, rightZ});
    _speakerIcon->drawWithin(g, frontRightArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    auto const listenZ         = frontView.getBottom() - room.listenPosition.z / scaleFactor;
    auto const frontListenArea = iconRect.withCentre({listenX, listenZ});
    _headIcon->drawWithin(g, frontListenArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    // DBG("Reflection (right): " + juce::String {reflectionRightSpeaker(room)});
    // DBG("Reflection Far (left): " + juce::String {reflectionLeftSpeakerFar(room)});
    // DBG("Reflection Far (right): " + juce::String {reflectionRightSpeakerFar(room)});
}

void FirstReflectionsView::resized()
{
    auto area = getLocalBounds().reduced(10);
    _drawArea = area.removeFromRight(area.proportionOfWidth(0.5)).reduced(10);

    area.reduce(10, 10);

    _renderProperties.setBounds(area.removeFromBottom(area.proportionOfHeight(0.2)).reduced(5));
    _roomProperties.setBounds(area.reduced(5));
}

auto FirstReflectionsView::connectValuesToTree() -> void
{
    _iconSize.getValueTree().addListener(this);

    _roomLength.getValueTree().addListener(this);
    _roomWidth.getValueTree().addListener(this);
    _roomHeight.getValueTree().addListener(this);

    _listenX.getValueTree().addListener(this);
    _listenY.getValueTree().addListener(this);
    _listenZ.getValueTree().addListener(this);

    _leftX.getValueTree().addListener(this);
    _leftY.getValueTree().addListener(this);
    _leftZ.getValueTree().addListener(this);

    _rightX.getValueTree().addListener(this);
    _rightY.getValueTree().addListener(this);
    _rightZ.getValueTree().addListener(this);

    _renderLeftReflections.getValueTree().addListener(this);
    _renderRightReflections.getValueTree().addListener(this);

    _roomProperties.clear();
    _renderProperties.clear();

    _roomProperties.addSection(
        "General", juce::Array<juce::PropertyComponent*>{
                       new juce::SliderPropertyComponent{_iconSize.getPropertyAsValue(), "Icon Size", 0.0, 100.0, 1.0},
                   });

    _roomProperties.addSection(
        "Room Dimensions",
        juce::Array<juce::PropertyComponent*>{
            new juce::SliderPropertyComponent{_roomLength.getPropertyAsValue(), "Length", 0.0, 1000.0, 1.0},
            new juce::SliderPropertyComponent{_roomWidth.getPropertyAsValue(), "Width", 0.0, 1000.0, 1.0},
            new juce::SliderPropertyComponent{_roomHeight.getPropertyAsValue(), "Height", 0.0, 1000.0, 1.0},
        });

    _roomProperties.addSection(
        "Listen Position", juce::Array<juce::PropertyComponent*>{
                               new juce::SliderPropertyComponent{_listenX.getPropertyAsValue(), "X", 0.0, 1000.0, 1.0},
                               new juce::SliderPropertyComponent{_listenY.getPropertyAsValue(), "Y", 0.0, 1000.0, 1.0},
                               new juce::SliderPropertyComponent{_listenZ.getPropertyAsValue(), "Z", 0.0, 1000.0, 1.0},
                           });

    _roomProperties.addSection(
        "Left Speaker", juce::Array<juce::PropertyComponent*>{
                            new juce::SliderPropertyComponent{_leftX.getPropertyAsValue(), "X", 0.0, 1000.0, 1.0},
                            new juce::SliderPropertyComponent{_leftY.getPropertyAsValue(), "Y", 0.0, 1000.0, 1.0},
                            new juce::SliderPropertyComponent{_leftZ.getPropertyAsValue(), "Z", 0.0, 1000.0, 1.0},
                        });

    _roomProperties.addSection(
        "Right Speaker", juce::Array<juce::PropertyComponent*>{
                             new juce::SliderPropertyComponent{_rightX.getPropertyAsValue(), "X", 0.0, 1000.0, 1.0},
                             new juce::SliderPropertyComponent{_rightY.getPropertyAsValue(), "Y", 0.0, 1000.0, 1.0},
                             new juce::SliderPropertyComponent{_rightZ.getPropertyAsValue(), "Z", 0.0, 1000.0, 1.0},
                         });

    _renderProperties.addSection(
        "First Reflections",
        juce::Array<juce::PropertyComponent*>{
            new juce::BooleanPropertyComponent{_renderLeftReflections.getPropertyAsValue(), "Left", "Draw"},
            new juce::BooleanPropertyComponent{_renderRightReflections.getPropertyAsValue(), "Right", "Draw"},
        });
}

auto FirstReflectionsView::valueTreePropertyChanged(juce::ValueTree& /*tree*/, juce::Identifier const& /*property*/)
    -> void
{
    repaint();
}
}  // namespace ra
