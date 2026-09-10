#pragma once
#include "raylib.h"

/// Đại diện cho 1 nút bấm dạng texture (2 trạng thái: idle / selected).
/// Không tự sở hữu Texture2D — chỉ giữ con trỏ tới texture do AssetManager quản lý,
/// nên vòng đời AssetManager phải dài hơn vòng đời Button (luôn đúng vì AssetManager
/// sống trong Game suốt vòng đời chương trình).
class Button {
public:
    Button() = default;

    void Init(const Texture2D& idleTexture, const Texture2D& selectedTexture, Vector2 position);

    /// Gọi mỗi frame để cập nhật trạng thái hover theo vị trí chuột hiện tại.
    void UpdateHover();

    bool IsHovered() const { return isHovered; }

    /// true nếu đang hover VÀ vừa nhấn chuột trái trong frame này.
    bool IsClicked() const;

    void Draw(bool isSelected) const;

    Rectangle GetBounds() const { return bounds; }

private:
    const Texture2D* idleTexture = nullptr;
    const Texture2D* selectedTexture = nullptr;
    Vector2 position{ 0, 0 };
    Rectangle bounds{ 0, 0, 0, 0 };
    bool isHovered = false;
};