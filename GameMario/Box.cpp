#include "Box.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

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

void CBox::Render()
{
	if (this->height <= 0 || this->width <= 0) return;
	CSprites* s = CSprites::GetInstance();

	int idTopLeft, idTopCenter, idTopRight;
	int idMiddleLeft, idMiddleCenter, idMiddleRight;
	int idBottomLeft, idBottomCenter, idBottomRight;

	switch (this->color)
	{
	case 0: // Orange
		idTopLeft = SPRITE_ID_ORANGE_TOP_LEFT;
		idTopCenter = SPRITE_ID_ORANGE_TOP_CENTER;
		idTopRight = SPRITE_ID_ORANGE_TOP_RIGHT;
		idMiddleLeft = SPRITE_ID_ORANGE_MIDDLE_LEFT;
		idMiddleCenter = SPRITE_ID_ORANGE_MIDDLE_CENTER;
		idMiddleRight = SPRITE_ID_ORANGE_MIDDLE_RIGHT;
		idBottomLeft = SPRITE_ID_ORANGE_BOTTOM_LEFT;
		idBottomCenter = SPRITE_ID_ORANGE_BOTTOM_CENTER;
		idBottomRight = SPRITE_ID_ORANGE_BOTTOM_RIGHT;
		break;
	case 1: // Blue
		idTopLeft = SPRITE_ID_BLUE_TOP_LEFT;
		idTopCenter = SPRITE_ID_BLUE_TOP_CENTER;
		idTopRight = SPRITE_ID_BLUE_TOP_RIGHT;
		idMiddleLeft = SPRITE_ID_BLUE_MIDDLE_LEFT;
		idMiddleCenter = SPRITE_ID_BLUE_MIDDLE_CENTER;
		idMiddleRight = SPRITE_ID_BLUE_MIDDLE_RIGHT;
		idBottomLeft = SPRITE_ID_BLUE_BOTTOM_LEFT;
		idBottomCenter = SPRITE_ID_BLUE_BOTTOM_CENTER;
		idBottomRight = SPRITE_ID_BLUE_BOTTOM_RIGHT;
		break;
	case 2: // Green
		idTopLeft = SPRITE_ID_GREEN_TOP_LEFT;
		idTopCenter = SPRITE_ID_GREEN_TOP_CENTER;
		idTopRight = SPRITE_ID_GREEN_TOP_RIGHT;
		idMiddleLeft = SPRITE_ID_GREEN_MIDDLE_LEFT;
		idMiddleCenter = SPRITE_ID_GREEN_MIDDLE_CENTER;
		idMiddleRight = SPRITE_ID_GREEN_MIDDLE_RIGHT;
		idBottomLeft = SPRITE_ID_GREEN_BOTTOM_LEFT;
		idBottomCenter = SPRITE_ID_GREEN_BOTTOM_CENTER;
		idBottomRight = SPRITE_ID_GREEN_BOTTOM_RIGHT;
		break;
	case 3: // White
		idTopLeft = SPRITE_ID_WHITE_TOP_LEFT;
		idTopCenter = SPRITE_ID_WHITE_TOP_CENTER;
		idTopRight = SPRITE_ID_WHITE_TOP_RIGHT;
		idMiddleLeft = SPRITE_ID_WHITE_MIDDLE_LEFT;
		idMiddleCenter = SPRITE_ID_WHITE_MIDDLE_CENTER;
		idMiddleRight = SPRITE_ID_WHITE_MIDDLE_RIGHT;
		// Missing bottom row sprites for white, use middle row instead
		idBottomLeft = SPRITE_ID_BLANK;
		idBottomCenter = SPRITE_ID_BLANK;
		idBottomRight = SPRITE_ID_BLANK;
		break;
	case 4: // Black
		idTopLeft = SPRITE_ID_BLACK_TOP_LEFT;
		idTopCenter = SPRITE_ID_BLACK_TOP_CENTER;
		idTopRight = SPRITE_ID_BLACK_TOP_RIGHT;
		idMiddleLeft = SPRITE_ID_BLACK_MIDDLE_LEFT;
		idMiddleCenter = SPRITE_ID_BLACK_MIDDLE_CENTER;
		idMiddleRight = SPRITE_ID_BLACK_MIDDLE_RIGHT;
		idBottomLeft = SPRITE_ID_BLACK_BOTTOM_LEFT;
		idBottomCenter = SPRITE_ID_BLACK_BOTTOM_CENTER;
		idBottomRight = SPRITE_ID_BLACK_BOTTOM_RIGHT;
		break;
	case 5: // Ending background
		idTopLeft = SPRITE_ID_ENDING_BORDER;
		idTopCenter = SPRITE_ID_ENDING_BACKGROUND;
		idTopRight = SPRITE_ID_ENDING_BACKGROUND;
		idMiddleLeft = SPRITE_ID_ENDING_BORDER;
		idMiddleCenter = SPRITE_ID_ENDING_BACKGROUND;
		idMiddleRight = SPRITE_ID_ENDING_BACKGROUND;
		idBottomLeft = SPRITE_ID_ENDING_BORDER;
		idBottomCenter = SPRITE_ID_ENDING_BACKGROUND;
		idBottomRight = SPRITE_ID_ENDING_BACKGROUND;
		break;
	default:
		idTopLeft = SPRITE_ID_BLANK;
		idTopCenter = SPRITE_ID_BLANK;
		idTopRight = SPRITE_ID_BLANK;
		idMiddleLeft = SPRITE_ID_BLANK;
		idMiddleCenter = SPRITE_ID_BLANK;
		idMiddleRight = SPRITE_ID_BLANK;
		idBottomLeft = SPRITE_ID_BLANK;
		idBottomCenter = SPRITE_ID_BLANK;
		idBottomRight = SPRITE_ID_BLANK;
		return;
	}

	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			float current_x = x + j * BOX_CELL_WIDTH;
			float current_y = y + i * BOX_CELL_HEIGHT;
			int spriteIdToDraw = -1; // Use -1 to indicate no sprite selected yet

			// Determine Row Type
			bool isTopRow = (i == 0);
			bool isBottomRow = (i == this->height - 1);
			bool isMiddleRow = (!isTopRow && !isBottomRow);

			// Determine Column Type
			bool isLeftCol = (j == 0);
			bool isRightCol = (j == this->width - 1);
			bool isCenterCol = (!isLeftCol && !isRightCol);

			// Select the correct sprite ID based on position
			if (isTopRow)
			{
				if (isLeftCol) spriteIdToDraw = idTopLeft;
				else if (isRightCol) spriteIdToDraw = idTopRight;
				else spriteIdToDraw = idTopCenter; // isCenterCol
			}
			else if (isBottomRow)
			{
				// Handle single-row boxes (height=1 means top row IS bottom row)
				if (this->height == 1) {
					if (isLeftCol) spriteIdToDraw = idTopLeft;
					else if (isRightCol) spriteIdToDraw = idTopRight;
					else spriteIdToDraw = idTopCenter;
				}
				// Handle multi-row boxes
				else {
					if (isLeftCol) spriteIdToDraw = idBottomLeft;
					else if (isRightCol) spriteIdToDraw = idBottomRight;
					else spriteIdToDraw = idBottomCenter; // isCenterCol
				}
			}
			else // isMiddleRow
			{
				if (isLeftCol) spriteIdToDraw = idMiddleLeft;
				else if (isRightCol) spriteIdToDraw = idMiddleRight;
				else spriteIdToDraw = idMiddleCenter; // isCenterCol
			}
			if (spriteIdToDraw != -1) {
				LPSPRITE sprite = s->Get(spriteIdToDraw);
				if (sprite) {
					sprite->Draw(current_x, current_y);
				}
				else {
					// Handle error: Sprite ID not found
					// Example: DebugOut(L"[ERROR] Sprite ID %d not found in CBox::Render\n", spriteIdToDraw);
				}
			}
		}
		float shadow_x = x + this->width * BOX_CELL_WIDTH;
		for (int i = 0; i < this->height; i++)
		{
			float shadow_y = y + i * BOX_CELL_HEIGHT;
			int shadowSpriteId = -1;

			if (i == 0) {
				shadowSpriteId = SPRITE_ID_SHADOW_TOP_RIGHT;
			}
			else {
				shadowSpriteId = SPRITE_ID_SHADOW_MIDDLE_RIGHT;
			}

			if (shadowSpriteId != -1) {
				LPSPRITE shadowSprite = s->Get(shadowSpriteId);
				if (shadowSprite) {
					shadowSprite->Draw(shadow_x, shadow_y);
				}
				else {
					// Handle error: Shadow Sprite ID not found
					// Example: DebugOut(L"[ERROR] Shadow Sprite ID %d not found in CBox::Render\n", shadowSpriteId);
				}
			}
		}

	}

	//RenderBoundingBox();
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