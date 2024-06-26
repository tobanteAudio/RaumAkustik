#include "RoomEditor.hpp"

#include "tool/PropertyComponent.hpp"

#include <ra/acoustic/Room.hpp>

namespace ra {

static constexpr auto Materials = std::array<std::pair<char const*, std::array<double, 10>>, 18>{
    std::pair{                  "Painted concrete",     std::array{0.01, 0.01, 0.01, 0.05, 0.06, 0.07, 0.09, 0.08, 0.08, 0.08}},
    std::pair{                   "Coarse concrete",     std::array{0.36, 0.36, 0.36, 0.44, 0.31, 0.29, 0.39, 0.25, 0.25, 0.25}},
    std::pair{                        "Wood floor",      std::array{0.15, 0.15, 0.15, 0.11, 0.1, 0.07, 0.06, 0.07, 0.07, 0.07}},
    std::pair{                    "Vinyl flooring",     std::array{0.03, 0.03, 0.03, 0.04, 0.05, 0.04, 0.05, 0.05, 0.05, 0.05}},
    std::pair{                      "Window glass",     std::array{0.35, 0.35, 0.35, 0.25, 0.18, 0.12, 0.07, 0.04, 0.04, 0.04}},
    std::pair{                       "Plate glass",     std::array{0.18, 0.18, 0.18, 0.06, 0.04, 0.03, 0.02, 0.02, 0.02, 0.02}},
    std::pair{                         "6mm glass",        std::array{0.1, 0.1, 0.1, 0.06, 0.04, 0.03, 0.02, 0.02, 0.02, 0.02}},
    std::pair{                         "Brickwork",     std::array{0.05, 0.05, 0.05, 0.04, 0.02, 0.04, 0.05, 0.05, 0.05, 0.05}},
    std::pair{       "Drapes 1/2 area. 15oz/sq yd",     std::array{0.07, 0.07, 0.07, 0.37, 0.49, 0.81, 0.65, 0.54, 0.54, 0.54}},
    std::pair{    "Foam backed carpet on concrete",          std::array{0.05, 0.05, 0.05, 0.16, 0.44, 0.7, 0.6, 0.4, 0.4, 0.4}},
    std::pair{"Carpet + foam underlay on concrete",           std::array{0.15, 0.15, 0.15, 0.25, 0.5, 0.6, 0.7, 0.8, 0.8, 0.8}},
    std::pair{                  "Plaster on brick", std::array{0.013, 0.013, 0.013, 0.015, 0.02, 0.03, 0.04, 0.05, 0.05, 0.05}},
    std::pair{"9mm Plasterboard over 20mm air gap",         std::array{0.3, 0.3, 0.3, 0.2, 0.15, 0.05, 0.05, 0.05, 0.05, 0.05}},
    std::pair{                      "Breeze block",           std::array{0.25, 0.25, 0.25, 0.4, 0.6, 0.5, 0.75, 0.5, 0.5, 0.5}},
    std::pair{                "50mm Acoustic Foam",          std::array{0.08, 0.08, 0.08, 0.25, 0.6, 0.9, 0.95, 0.9, 0.9, 0.9}},
    std::pair{               "100mm Acoustic Foam",         std::array{0.2, 0.2, 0.2, 0.7, 0.99, 0.99, 0.99, 0.99, 0.99, 0.99}},
    std::pair{   "50mm Mineral Wool (Med Density)",              std::array{0.2, 0.2, 0.2, 0.45, 0.7, 0.8, 0.8, 0.8, 0.8, 0.8}},
    std::pair{                 "LF panel absorber",      std::array{0.28, 0.28, 0.28, 0.22, 0.17, 0.09, 0.1, 0.11, 0.11, 0.11}},
};

RoomEditor::RoomEditor(juce::ValueTree vt, juce::UndoManager* um)
    : _undoManager{um}
    , _roomTree{vt.getOrCreateChildWithName("Room", um)}
{
    _iconSize.setDefault(20.0);

    addAndMakeVisible(_roomProperties);
    addAndMakeVisible(_renderProperties);

    _roomTree.addListener(this);
    buildProperties();
    setSize(600, 400);
}

auto RoomEditor::getRoomLayout() const -> RoomLayout
{
    auto const dim = RoomDimensions{
        double{_roomLength} * si::metre,
        double{_roomWidth} * si::metre,
        double{_roomHeight} * si::metre,
    };
    auto const listener = glm::dvec3(double{_listenX}, double{_listenY}, double{_listenZ});
    auto const speakers = std::array{
        glm::dvec3(double{_leftX}, double{_leftY}, double{_leftZ}),
        glm::dvec3(double{_rightX}, double{_rightY}, double{_rightZ}),
    };
    return {
        .dimensions     = dim,
        .speakers       = speakers,
        .listenPosition = listener,
    };
}

void RoomEditor::paint(juce::Graphics& g)
{
    auto* const lnf = dynamic_cast<juce::LookAndFeel_V4*>(&getLookAndFeel());
    jassert(lnf != nullptr);

    auto const iconSize = std::max<double>(_iconSize, 1.0);
    auto const iconRect = juce::Rectangle{0.0, 0.0, iconSize, iconSize};

    auto const room       = getRoomLayout();
    auto const roomLength = room.dimensions.length.numerical_value_in(si::metre);
    auto const roomWidth  = room.dimensions.width.numerical_value_in(si::metre);
    auto const roomHeight = room.dimensions.height.numerical_value_in(si::metre);
    if (roomLength == 0.0) {
        return;
    }
    if (roomWidth == 0.0) {
        return;
    }
    if (roomHeight == 0.0) {
        return;
    }

    auto totalArea = _drawArea;
    auto bgColor   = lnf->getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::widgetBackground);
    g.setColour(bgColor);
    g.fillRect(_drawArea);

    ///////////////////////////////////////// TOP
    auto const topViewArea  = totalArea.removeFromTop(totalArea.proportionOfHeight(0.66));
    auto const area         = topViewArea.toDouble();
    auto const scaleFactor  = roomLength / (area.getHeight() * 0.9);
    auto const roomLengthPx = roomLength / scaleFactor;
    auto const roomWidthPx  = roomWidth / scaleFactor;
    auto const topViewRoom  = juce::Rectangle{area.getX(), area.getY(), roomWidthPx, roomLengthPx}.withCentre(
        topViewArea.getCentre().toDouble()
    );

    g.setColour(juce::Colours::black);
    g.drawRect(topViewRoom.toFloat(), 8.0F);

    auto const leftX    = topViewRoom.getX() + room.speakers[0].x / scaleFactor;
    auto const leftY    = topViewRoom.getY() + room.speakers[0].y / scaleFactor;
    auto const leftArea = iconRect.withCentre({leftX, leftY});
    _speakerIcon->drawWithin(g, leftArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    auto const rightX    = topViewRoom.getX() + room.speakers[1].x / scaleFactor;
    auto const rightY    = topViewRoom.getY() + room.speakers[1].y / scaleFactor;
    auto const rightArea = iconRect.withCentre({rightX, rightY});
    _speakerIcon->drawWithin(g, rightArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    auto const listenX    = topViewRoom.getX() + room.listenPosition.x / scaleFactor;
    auto const listenY    = topViewRoom.getY() + room.listenPosition.y / scaleFactor;
    auto const listenArea = iconRect.withCentre({listenX, listenY});
    _headIcon->drawWithin(g, listenArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    ///////////////////////////////////////// FRONT
    auto const frontArea    = totalArea.toDouble();
    auto const roomHeightPx = roomHeight / scaleFactor;
    auto const frontView    = juce::Rectangle{0.0, 0.0, roomWidthPx, roomHeightPx}.withCentre(frontArea.getCentre());
    g.drawRect(frontView.toFloat(), 8.0F);

    auto const leftZ         = frontView.getBottom() - room.speakers[0].z / scaleFactor;
    auto const frontLeftArea = iconRect.withCentre({leftX, leftZ});
    _speakerIcon->drawWithin(g, frontLeftArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    auto const rightZ         = frontView.getBottom() - room.speakers[1].z / scaleFactor;
    auto const frontRightArea = iconRect.withCentre({rightX, rightZ});
    _speakerIcon->drawWithin(g, frontRightArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);

    auto const listenZ         = frontView.getBottom() - room.listenPosition.z / scaleFactor;
    auto const frontListenArea = iconRect.withCentre({listenX, listenZ});
    _headIcon->drawWithin(g, frontListenArea.toFloat(), juce::RectanglePlacement::centred, 1.0F);
}

void RoomEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    _drawArea = area.removeFromRight(area.proportionOfWidth(0.5)).reduced(10);

    area.reduce(10, 10);

    _renderProperties.setBounds(area.removeFromBottom(area.proportionOfHeight(0.2)).reduced(5));
    _roomProperties.setBounds(area.reduced(5));
}

auto RoomEditor::buildProperties() -> void
{
    using juce::ChoicePropertyComponent;
    using std::make_unique;

    auto const names = [] {
        auto n = juce::StringArray{};
        for (auto const& m : Materials) {
            n.add(m.first);
        }
        return n;
    }();
    auto const vars = [&] {
        auto v = juce::Array<juce::var>{};
        for (auto const& m : names) {
            v.add(m);
        }
        return v;
    }();
    auto const surfaces = juce::Array<juce::PropertyComponent*>{
        make_unique<ChoicePropertyComponent>(_materialLeft.getPropertyAsValue(), "Left", names, vars).release(),
        make_unique<ChoicePropertyComponent>(_materialRight.getPropertyAsValue(), "Right", names, vars).release(),
        make_unique<ChoicePropertyComponent>(_materialFront.getPropertyAsValue(), "Front", names, vars).release(),
        make_unique<ChoicePropertyComponent>(_materialBack.getPropertyAsValue(), "Back", names, vars).release(),
        make_unique<ChoicePropertyComponent>(_materialCeiling.getPropertyAsValue(), "Ceiling", names, vars).release(),
        make_unique<ChoicePropertyComponent>(_materialFloor.getPropertyAsValue(), "Floor", names, vars).release(),
    };

    _roomProperties.addSection(
        "General",
        juce::Array<juce::PropertyComponent*>{
            makeProperty<juce::SliderPropertyComponent>(_iconSize, "Icon Size", 0.0, 100.0, 1.0),
        }
    );

    _roomProperties.addSection(
        "Room Dimensions",
        juce::Array<juce::PropertyComponent*>{
            makeProperty<juce::SliderPropertyComponent>(_roomLength, "Length", 0.0, 10.0, 0.01),
            makeProperty<juce::SliderPropertyComponent>(_roomWidth, "Width", 0.0, 10.0, 0.01),
            makeProperty<juce::SliderPropertyComponent>(_roomHeight, "Height", 0.0, 10.0, 0.01),
        }
    );

    _roomProperties.addSection("Surfaces", surfaces);

    _roomProperties.addSection(
        "Listen Position",
        juce::Array<juce::PropertyComponent*>{
            makeProperty<juce::SliderPropertyComponent>(_listenX, "X", 0.0, 10.0, 0.01),
            makeProperty<juce::SliderPropertyComponent>(_listenY, "Y", 0.0, 10.0, 0.01),
            makeProperty<juce::SliderPropertyComponent>(_listenZ, "Z", 0.0, 10.0, 0.01),
        }
    );

    _roomProperties.addSection(
        "Left Speaker",
        juce::Array<juce::PropertyComponent*>{
            makeProperty<juce::SliderPropertyComponent>(_leftX, "X", 0.0, 10.0, 0.01),
            makeProperty<juce::SliderPropertyComponent>(_leftY, "Y", 0.0, 10.0, 0.01),
            makeProperty<juce::SliderPropertyComponent>(_leftZ, "Z", 0.0, 10.0, 0.01),
        }
    );

    _roomProperties.addSection(
        "Right Speaker",
        juce::Array<juce::PropertyComponent*>{
            makeProperty<juce::SliderPropertyComponent>(_rightX, "X", 0.0, 10.0, 0.01),
            makeProperty<juce::SliderPropertyComponent>(_rightY, "Y", 0.0, 10.0, 0.01),
            makeProperty<juce::SliderPropertyComponent>(_rightZ, "Z", 0.0, 10.0, 0.01),
        }
    );

    _renderProperties.addSection(
        "First Reflections",
        juce::Array<juce::PropertyComponent*>{
            makeProperty<juce::BooleanPropertyComponent>(_renderLeftReflections, "Left", "Draw"),
            makeProperty<juce::BooleanPropertyComponent>(_renderRightReflections, "Right", "Draw"),
        }
    );
}

auto RoomEditor::valueTreePropertyChanged(juce::ValueTree& /*tree*/, juce::Identifier const& /*property*/) -> void
{
    repaint();
}

}  // namespace ra
