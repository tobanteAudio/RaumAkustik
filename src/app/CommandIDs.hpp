#pragma once

namespace mc
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
}  // namespace mc
