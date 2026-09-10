#include "ui/Button.h"

void Button::Init(const Texture2D& idleTex, const Texture2D& selectedTex, Vector2 pos) {
    idleTexture = &idleTex;
    selectedTexture = &selectedTex;
    position = pos;
    // idle và selected LUÔN cùng kích thước (đã đảm bảo lúc thiết kế ảnh),
    // nên dùng idle làm chuẩn để tính vùng va chạm chuột.
    bounds = { position.x, position.y, (float)idleTexture->width, (float)idleTexture->height };
}

void Button::UpdateHover() {
    isHovered = CheckCollisionPointRec(GetMousePosition(), bounds);
}

bool Button::IsClicked() const {
    return isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void Button::Draw(bool isSelected) const {
    const Texture2D& tex = isSelected ? *selectedTexture : *idleTexture;
    DrawTextureV(tex, position, WHITE);
}