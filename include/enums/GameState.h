#pragma once

/// Trạng thái tổng của toàn bộ game — dùng bởi Game để biết đang chạy state nào.
enum class GameState {
    MENU,
    LEVEL_SELECT,
    LEVEL_EDITOR,
    PLAYING,
    GAME_OVER
};