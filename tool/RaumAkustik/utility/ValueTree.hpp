#pragma once

#include <juce_data_structures/juce_data_structures.h>

namespace ra {

inline auto setPropertyIfNotExist(juce::ValueTree& tree, juce::Identifier const& id, juce::var const& value) -> void
{
    if (tree.hasProperty(id)) {
        return;
    }
    tree.setProperty(id, value, nullptr);
}

}  // namespace ra
