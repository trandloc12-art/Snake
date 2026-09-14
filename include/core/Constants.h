#pragma once

// ============================================================
// Các hằng số dùng chung cho TOÀN BỘ dự án
// ============================================================

// ---------- Cửa sổ game ----------
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1060;
constexpr int TARGET_FPS = 60;
constexpr const char* WINDOW_TITLE = "Snake Game";

// ---------- Sprite / Rendering ----------
constexpr int TILE_SOURCE_SIZE = 64;
constexpr int DEFAULT_CELL_SIZE = 64; // kích thước ô lưới (pixel) để vẽ texture, có thể scale lên khi window to

// ---------- Animation rắn (flipbook) ----------
// Vị trí các đốt luôn SNAP đúng ô lưới (không nội suy nữa) - cảm giác "di chuyển mượt"
// đến hoàn toàn từ việc đổi khung hình theo thời gian thực (giống băng chuyền).
constexpr float SNAKE_ANIM_FPS_HEAD = 4.0f;  // đầu: 4 khung hình/giây
constexpr float SNAKE_ANIM_FPS_BODY = 6.0f;  // thân/đuôi/góc cua: giữ nguyên tốc độ cũ (chỉnh nếu cần)

constexpr int   SNAKE_ANIM_FRAMES_BODY = 6;            // số khung - dùng chung cho đầu / thân thẳng / đuôi / góc cua
                                                        // (mỗi loại nằm trong 1 thư mục con riêng: assets/textures/<ten>/<ten>_0.png..<ten>_7.png,
                                                        // ví dụ assets/textures/snake_body/snake_body_0.png..snake_body_7.png).
constexpr int SNAKE_ANIM_FRAMES_HEAD = 4;  // đầu rắn: 4 khung hình
constexpr float SNAKE_ANIM_PHASE_PER_SEGMENT = 0.5f;   // độ lệch pha (đơn vị: khung hình) giữa 2 đốt liền kề theo chỉ số i,
                                                        // tạo hiệu ứng hoạ văn "chảy" dọc thân từ đầu xuống đuôi. Có thể chỉnh để
                                                        // đổi tốc độ "chảy" của hiệu ứng mà không đụng tới SNAKE_ANIM_FPS.

// ---------- Gameplay: Rắn ----------
constexpr int DEFAULT_SNAKE_LENGTH = 3;
constexpr float DEFAULT_MOVE_INTERVAL = 0.2f; // thời gian giữa các bước di chuyển

// ---------- Gameplay: Level Editor ----------
constexpr int MAX_LEVEL_WIDTH = 100;
constexpr int MAX_LEVEL_HEIGHT = 100;
constexpr int MIN_LEVEL_WIDTH = 5;
constexpr int MIN_LEVEL_HEIGHT = 5;

// ---------- Đường dẫn thư mục ----------
// QUAN TRỌNG: level do Editor tạo cần đọc/ghi TRỰC TIẾP vào thư mục gốc project
// (không phải build/), để: (1) không bị mất khi xóa build/, (2) commit được lên Git,
// (3) LevelSelectState thấy ngay file mới mà không cần build lại.
// PROJECT_SOURCE_DIR được CMake nhúng lúc biên dịch (xem target_compile_definitions).
#ifdef PROJECT_SOURCE_DIR
    constexpr const char* LEVELS_DIRECTORY = PROJECT_SOURCE_DIR "assets/levels";
#else
    // Dự phòng nếu ai đó build không qua CMake (hiếm khi xảy ra) - dùng path tương đối cũ.
    constexpr const char* LEVELS_DIRECTORY = "assets/levels";
#endif

// Texture thì KHÔNG cần đổi - vẫn đọc từ build/assets/textures/ vì ảnh không đổi lúc chạy,
// việc CMake copy sẵn vào build/ là đủ, không cần đọc trực tiếp từ source.
constexpr const char* TEXTURES_DIRECTORY = "assets/textures/";

// Asset animation (flipbook) của rắn - snake_head/snake_body/snake_tail/snake_corner - nằm
// riêng ở đây, TÁCH KHỎI TEXTURES_DIRECTORY (nơi vẫn chỉ chứa wall/food/playing_background).
constexpr const char* ANIMATIONS_DIRECTORY = "assets/animations/";

constexpr const char* DEFAULT_LEVEL_FILE = "level1.txt"; // chỉ tên file, LEVELS_DIRECTORY đã có path đầy đủ

// ---------- Skin rắn ----------
// ANIMATIONS_DIRECTORY (vd assets/animations/classic/, assets/animations/neon/).
// Thêm skin mới sau này: thêm 1 dòng vào mảng này + tạo đủ thư mục ảnh tương ứng,
// không cần sửa gì thêm ở AssetManager hay SkinSelectState.
constexpr const char* SNAKE_SKINS[] = { "classic", "neon" };
constexpr int SNAKE_SKIN_COUNT = 2;

constexpr const char* DEFAULT_SNAKE_SKIN = "classic";