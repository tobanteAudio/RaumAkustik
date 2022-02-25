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
        gotoTabLeft,
        gotoTabRight,
        about,
    };
};
}  // namespace mc
