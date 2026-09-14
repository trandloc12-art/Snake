#pragma once
#include <unordered_map>
#include <string>
#include "raylib.h"

/// Load và quản lý tập trung toàn bộ texture của game.
/// Sở hữu tài nguyên GPU (Texture2D) -> KHÔNG được phép copy, chỉ nên có
/// 1 instance duy nhất (sống trong Game), truyền xuống nơi khác bằng tham chiếu.
class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager();

    // Cấm copy để tránh 2 object cùng giữ 1 texture.id -> double-free khi UnloadAll().
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    /// Load tất cả texture cần dùng trong game — gọi 1 LẦN DUY NHẤT lúc khởi động.
    void LoadAll();

    /// Lấy texture theo tên (đã load sẵn) để vẽ. Dừng chương trình (assert) ở bản Debug
    /// nếu tên không tồn tại — đây gần như luôn là lỗi gõ sai tên lúc code.
    const Texture2D& GetTexture(const std::string& name) const;

    /// Giải phóng toàn bộ texture — gọi thủ công nếu cần giải phóng sớm,
    /// destructor cũng tự gọi hàm này nên không bắt buộc phải gọi tay.
    void UnloadAll();

private:
    /// Load 1 texture, kiểm tra load thành công và đúng kích thước chuẩn (TILE_SOURCE_SIZE),
    /// đồng thời bật lọc ảnh dạng "point" để giữ pixel sắc nét khi scale.
    void LoadTextureChecked(const std::string& name, const std::string& path, bool checkSize = true);

    /// Load 1 bộ khung hình animation (flipbook), nằm trong thư mục con "<name>_animation/"
    /// (thư mục có hậu tố "_animation", TÊN FILE bên trong thì KHÔNG có hậu tố này):
    /// "<dir>/<name>_animation/<name>_0.png" đến "<dir>/<name>_animation/<name>_(frameCount-1).png".
    /// Lưu vào map với key "<name>_<i>" (không có path/đuôi .png).
    /// Dùng cho đầu/thân/đuôi/góc cua của rắn - mỗi loại hiện có SNAKE_ANIM_FRAMES_BODY khung.
    void LoadAnimationFrames(const std::string& name, const std::string& dir, int frameCount);

    std::unordered_map<std::string, Texture2D> textures;
};