#include "Box.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

#include "debug.h"

void CBox::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPTEXTURE bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	float cx, cy;
	CGame::GetInstance()->GetCamPos(cx, cy);

	float xx = x - BOX_CELL_WIDTH / 2 + rect.right / 2;
	float yy = y - BOX_CELL_HEIGHT / 2 + rect.bottom / 2;

	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CBox::RenderBox(CSprites* s)
{
	if (width <= 0 || height <= 0) return;

	// Sprite IDs for each position based on color
	struct BoxSprites {
		int topLeft, topCenter, topRight;
		int middleLeft, middleCenter, middleRight;
		int bottomLeft, bottomCenter, bottomRight;
	};

	BoxSprites sprites;
	switch (color)
	{
	case 0: // Orange
		sprites = {
			SPRITE_ID_ORANGE_TOP_LEFT, SPRITE_ID_ORANGE_TOP_CENTER, SPRITE_ID_ORANGE_TOP_RIGHT,
			SPRITE_ID_ORANGE_MIDDLE_LEFT, SPRITE_ID_ORANGE_MIDDLE_CENTER, SPRITE_ID_ORANGE_MIDDLE_RIGHT,
			SPRITE_ID_ORANGE_BOTTOM_LEFT, SPRITE_ID_ORANGE_BOTTOM_CENTER, SPRITE_ID_ORANGE_BOTTOM_RIGHT
		};
		break;
	case 1: // Blue
		sprites = {
			SPRITE_ID_BLUE_TOP_LEFT, SPRITE_ID_BLUE_TOP_CENTER, SPRITE_ID_BLUE_TOP_RIGHT,
			SPRITE_ID_BLUE_MIDDLE_LEFT, SPRITE_ID_BLUE_MIDDLE_CENTER, SPRITE_ID_BLUE_MIDDLE_RIGHT,
			SPRITE_ID_BLUE_BOTTOM_LEFT, SPRITE_ID_BLUE_BOTTOM_CENTER, SPRITE_ID_BLUE_BOTTOM_RIGHT
		};
		break;
	case 2: // Green
		sprites = {
			SPRITE_ID_GREEN_TOP_LEFT, SPRITE_ID_GREEN_TOP_CENTER, SPRITE_ID_GREEN_TOP_RIGHT,
			SPRITE_ID_GREEN_MIDDLE_LEFT, SPRITE_ID_GREEN_MIDDLE_CENTER, SPRITE_ID_GREEN_MIDDLE_RIGHT,
			SPRITE_ID_GREEN_BOTTOM_LEFT, SPRITE_ID_GREEN_BOTTOM_CENTER, SPRITE_ID_GREEN_BOTTOM_RIGHT
		};
		break;
	case 3: // White
		sprites = {
			SPRITE_ID_WHITE_TOP_LEFT, SPRITE_ID_WHITE_TOP_CENTER, SPRITE_ID_WHITE_TOP_RIGHT,
			SPRITE_ID_WHITE_MIDDLE_LEFT, SPRITE_ID_WHITE_MIDDLE_CENTER, SPRITE_ID_WHITE_MIDDLE_RIGHT,
			SPRITE_ID_WHITE_MIDDLE_LEFT, SPRITE_ID_WHITE_MIDDLE_CENTER, SPRITE_ID_WHITE_MIDDLE_RIGHT // Use middle row for bottom
		};
		break;
	case 4: // Black
		sprites = {
			SPRITE_ID_BLACK_TOP_LEFT, SPRITE_ID_BLACK_TOP_CENTER, SPRITE_ID_BLACK_TOP_RIGHT,
			SPRITE_ID_BLACK_MIDDLE_LEFT, SPRITE_ID_BLACK_MIDDLE_CENTER, SPRITE_ID_BLACK_MIDDLE_RIGHT,
			SPRITE_ID_BLACK_BOTTOM_LEFT, SPRITE_ID_BLACK_BOTTOM_CENTER, SPRITE_ID_BLACK_BOTTOM_RIGHT
		};
		break;
	case 5: // Ending background
		sprites = {
			SPRITE_ID_ENDING_BORDER, SPRITE_ID_ENDING_BACKGROUND, SPRITE_ID_ENDING_BACKGROUND,
			SPRITE_ID_ENDING_BORDER, SPRITE_ID_ENDING_BACKGROUND, SPRITE_ID_ENDING_BACKGROUND,
			SPRITE_ID_ENDING_BORDER, SPRITE_ID_ENDING_BACKGROUND, SPRITE_ID_ENDING_BACKGROUND
		};
		break;
	default:
		return; // No rendering for invalid color
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			float current_x = x + j * BOX_CELL_WIDTH;
			float current_y = y + i * BOX_CELL_HEIGHT;
			int spriteId = -1;

			bool isTopRow = (i == 0);
			bool isBottomRow = (i == height - 1);
			bool isLeftCol = (j == 0);
			bool isRightCol = (j == width - 1);

			if (isTopRow || (height == 1)) // Single-row boxes use top sprites
			{
				spriteId = isLeftCol ? sprites.topLeft :
					isRightCol ? sprites.topRight : sprites.topCenter;
			}
			else if (isBottomRow)
			{
				spriteId = isLeftCol ? sprites.bottomLeft :
					isRightCol ? sprites.bottomRight : sprites.bottomCenter;
			}
			else // Middle row
			{
				spriteId = isLeftCol ? sprites.middleLeft :
					isRightCol ? sprites.middleRight : sprites.middleCenter;
			}

			if (spriteId != -1)
			{
				LPSPRITE sprite = s->Get(spriteId);
				if (sprite)
				{
					sprite->Draw(current_x, current_y);
				}
				else
				{
					//DebugOut(L"[ERROR] Box Sprite ID %d not found in CBox::RenderBox\n", spriteId);
				}
			}
		}
	}
}

void CBox::RenderShadow(CSprites* s)
{
	if (width <= 0 || height <= 0) return;

	// Right edge shadow (always rendered, outside right border)
	for (int j = 0; j < height; j++)
	{
		float shadow_x = x + width * BOX_CELL_WIDTH; // Right of the box's rightmost column
		float shadow_y = y + j * BOX_CELL_HEIGHT;   // Aligned with box rows
		int shadowSpriteId = -1;

		if (j == 0)
			shadowSpriteId = SPRITE_ID_SHADOW_TOP_RIGHT;      // Top-right corner
		else
			shadowSpriteId = SPRITE_ID_SHADOW_MIDDLE_RIGHT;   // Middle-right cells

		if (shadowSpriteId != -1)
		{
			LPSPRITE shadowSprite = s->Get(shadowSpriteId);
			if (shadowSprite)
			{
				shadowSprite->Draw(shadow_x, shadow_y);
			}
			else
			{
				DebugOut(L"[ERROR] Shadow Sprite ID %d not found in CBox::RenderShadow\n", shadowSpriteId);
			}
		}
	}

	// Bottom shadow (rendered only when bottomShadow == 1, outside bottom border)
	if (bottomShadow == 1)
	{
		for (int i = 0; i <= width; i++)
		{
			float shadow_x = x + i * BOX_CELL_WIDTH;         // Aligned with box columns
			float shadow_y = y + height * BOX_CELL_HEIGHT;   // Below the box's bottommost row
			int shadowSpriteId = -1;

			if (i == 0)
				shadowSpriteId = SPRITE_ID_SHADOW_BOTTOM_LEFT;    // Bottom-left corner
			else if (i == width)
				shadowSpriteId = SPRITE_ID_SHADOW_BOTTOM_RIGHT;   // Bottom-right corner (matches right edge)
			else
				shadowSpriteId = SPRITE_ID_SHADOW_BOTTOM_CENTER;  // Bottom-center cells

			if (shadowSpriteId != -1)
			{
				LPSPRITE shadowSprite = s->Get(shadowSpriteId);
				if (shadowSprite)
				{
					shadowSprite->Draw(shadow_x, shadow_y);
				}
				else
				{
					DebugOut(L"[ERROR] Shadow Sprite ID %d not found in CBox::RenderShadow\n", shadowSpriteId);
				}
			}
		}
	}
}

void CBox::Render()
{
	CSprites* s = CSprites::GetInstance();
	RenderBox(s);
	RenderShadow(s);
}

void CBox::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BOX_CELL_WIDTH / 2;
	t = y - BOX_CELL_HEIGHT / 2;
	r = l + BOX_CELL_WIDTH * this->width;
	b = t + BOX_CELL_HEIGHT * this->height;
}

int CBox::IsDirectionColliable(float nx, float ny)
{
	if (nx == 0 && ny == -1) return 1;
	else return 0;
}