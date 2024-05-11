#pragma once

namespace ra
{
struct CommandIDs
{
    enum Values
    {
        unknown,
        open,
        save,
        saveAs,
        undo,
        redo,
        settings,
        gotoTabLeft,
        gotoTabRight,
        fullscreen,
        about,
    };
};
}  // namespace ra
