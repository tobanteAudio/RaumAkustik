#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

namespace ra {

template<typename Comp, typename... Args>
[[nodiscard]] auto makeProperty(juce::Value value, Args&&... args) -> Comp*
{
    return std::make_unique<Comp>(value, std::forward<Args>(args)...).release();
}

template<typename Comp, typename T, typename... Args>
[[nodiscard]] auto makeProperty(juce::CachedValue<T>& value, Args&&... args) -> Comp*
{
    return makeProperty<Comp>(value.getPropertyAsValue(), std::forward<Args>(args)...);
}

}  // namespace ra
