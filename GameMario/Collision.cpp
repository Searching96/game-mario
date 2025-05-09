#include "Collision.h"
#include "GameObject.h"

#include "TailWhip.h"
#include "BuffQBlock.h"
#include "CoinQBlock.h"
#include "WingedGoomba.h"
#include "Goomba.h"
#include "Koopa.h"
#include "PiranhaPlant.h"

#include "debug.h"

#define BLOCK_PUSH_FACTOR 0.01f

CCollision* CCollision::__instance = nullptr;

int CCollisionEvent::WasCollided() {
	return
		t >= 0.0f && t <= 1.0f && obj->IsDirectionColliable(nx, ny) == 1;
}

CCollision* CCollision::GetInstance()
{
	if (__instance == nullptr) __instance = new CCollision();
	return __instance;
}

/*
	SweptAABB
*/
void CCollision::SweptAABB(
	float ml, float mt, float mr, float mb,
	float dx, float dy,
	float sl, float st, float sr, float sb,
	float& t, float& nx, float& ny)
{

	float dx_entry, dx_exit, tx_entry, tx_exit;
	float dy_entry, dy_exit, ty_entry, ty_exit;

	float t_entry;
	float t_exit;

	t = -1.0f;			// no collision
	nx = ny = 0.0f;

	//
	// Broad-phase test 
	//

	float bl = dx > 0 ? ml : ml + dx;
	float bt = dy > 0 ? mt : mt + dy;
	float br = dx > 0 ? mr + dx : mr;
	float bb = dy > 0 ? mb + dy : mb;

	if (br < sl || bl > sr || bb < st || bt > sb) return;


	if (dx == 0 && dy == 0) return;		// moving object is not moving > obvious no collision

	if (dx > 0)
	{
		dx_entry = sl - mr;
		dx_exit = sr - ml;
	}
	else if (dx < 0)
	{
		dx_entry = sr - ml;
		dx_exit = sl - mr;
	}


	if (dy > 0)
	{
		dy_entry = st - mb;
		dy_exit = sb - mt;
	}
	else if (dy < 0)
	{
		dy_entry = sb - mt;
		dy_exit = st - mb;
	}

	if (dx == 0)
	{
		tx_entry = -9999999.0f;
		tx_exit = 99999999.0f;
	}
	else
	{
		tx_entry = dx_entry / dx;
		tx_exit = dx_exit / dx;
	}

	if (dy == 0)
	{
		ty_entry = -99999999999.0f;
		ty_exit = 99999999999.0f;
	}
	else
	{
		ty_entry = dy_entry / dy;
		ty_exit = dy_exit / dy;
	}


	if ((tx_entry < 0.0f && ty_entry < 0.0f) || tx_entry > 1.0f || ty_entry > 1.0f) return;

	t_entry = max(tx_entry, ty_entry);
	t_exit = min(tx_exit, ty_exit);

	if (t_entry > t_exit) return;

	t = t_entry;

	if (tx_entry > ty_entry)
	{
		ny = 0.0f;
		dx > 0 ? nx = -1.0f : nx = 1.0f;
	}
	else
	{
		nx = 0.0f;
		dy > 0 ? ny = -1.0f : ny = 1.0f;
	}

}

void CCollision::SweptAABB(float ml, float mt, float mr, float mb,
	float dx, float dy,
	float sl, float st, float sr, float sb,
	float& t, float& nx, float& ny, 
	LPGAMEOBJECT objSrc, LPGAMEOBJECT objDest)
{

	float dx_entry, dx_exit, tx_entry, tx_exit;
	float dy_entry, dy_exit, ty_entry, ty_exit;

	float t_entry;
	float t_exit;

	t = -1.0f;			// no collision
	nx = ny = 0.0f;

	if (dynamic_cast<CMario*>(objSrc) && dynamic_cast<CMushroom*>(objDest))
	{
		if (ml < sr && mr > sl && mt < sb && mb > st) {
			t = 0.0f;      // collision at the start of the frame
			nx = ny = 0.0f;
			return;
		}
	}

	if (dynamic_cast<CTailWhip*>(objSrc)) {
		if (ml < sr && mr > sl && mt < sb && mb > st) {
			t = 0.0f;      // collision at the start of the frame
			nx = ny = 0.0f;
			return;
		}
	}

	if (dynamic_cast<CKoopa*>(objSrc) && objDest->IsBlocking() == 1)
	{
		if (ml < sr && mr > sl && mt < sb && mb > st)
		{
			t = 0.0f;      // collision at the start of the frame
			nx = ny = 0.0f;
			return;
		}
	}

	if (dynamic_cast<CKoopa*>(objSrc) && (dynamic_cast<CGoomba*>(objDest)
		|| dynamic_cast<CWingedGoomba*>(objDest) || dynamic_cast<CPiranhaPlant*>(objDest)))
	{
		if (dynamic_cast<CKoopa*>(objSrc)->GetState() == KOOPA_STATE_SHELL_DYNAMIC)
		{
			if (ml < sr && mr > sl && mt < sb && mb > st)
			{
				t = 0.0f;      // collision at the start of the frame
				nx = ny = 0.0f;
				return;
			}
		}
	}

	if (dynamic_cast<CKoopa*>(objSrc) && (dynamic_cast<CGoomba*>(objDest)
		|| dynamic_cast<CWingedGoomba*>(objDest) || dynamic_cast<CPiranhaPlant*>(objDest)))
	{
		if (dynamic_cast<CKoopa*>(objSrc)->GetBeingHeld() == 1)
		{
			if (ml < sr && mr > sl && mt < sb && mb > st)
			{
				t = 0.0f;      // collision at the start of the frame
				nx = ny = 0.0f;
				return;
			}
		}
	}

	if (dynamic_cast<CKoopa*>(objDest) && (dynamic_cast<CGoomba*>(objSrc)
		|| dynamic_cast<CWingedGoomba*>(objSrc) || dynamic_cast<CPiranhaPlant*>(objSrc)))
	{
		if (dynamic_cast<CKoopa*>(objDest)->GetState() == KOOPA_STATE_SHELL_DYNAMIC)
		{
			if (ml < sr && mr > sl && mt < sb && mb > st)
			{
				t = 0.0f;      // collision at the start of the frame
				nx = ny = 0.0f;
				return;
			}
		}
	}

	//
	// Broad-phase test 
	//

	float bl = dx > 0 ? ml : ml + dx;
	float bt = dy > 0 ? mt : mt + dy;
	float br = dx > 0 ? mr + dx : mr;
	float bb = dy > 0 ? mb + dy : mb;

	if (br < sl || bl > sr || bb < st || bt > sb) return;


	if (dx == 0 && dy == 0) return;		// moving object is not moving > obvious no collision

	if (dx > 0)
	{
		dx_entry = sl - mr;
		dx_exit = sr - ml;
	}
	else if (dx < 0)
	{
		dx_entry = sr - ml;
		dx_exit = sl - mr;
	}


	if (dy > 0)
	{
		dy_entry = st - mb;
		dy_exit = sb - mt;
	}
	else if (dy < 0)
	{
		dy_entry = sb - mt;
		dy_exit = st - mb;
	}

	if (dx == 0)
	{
		tx_entry = -9999999.0f;
		tx_exit = 99999999.0f;
	}
	else
	{
		tx_entry = dx_entry / dx;
		tx_exit = dx_exit / dx;
	}

	if (dy == 0)
	{
		ty_entry = -99999999999.0f;
		ty_exit = 99999999999.0f;
	}
	else
	{
		ty_entry = dy_entry / dy;
		ty_exit = dy_exit / dy;
	}


	if ((tx_entry < 0.0f && ty_entry < 0.0f) || tx_entry > 1.0f || ty_entry > 1.0f) return;

	t_entry = max(tx_entry, ty_entry);
	t_exit = min(tx_exit, ty_exit);

	if (t_entry > t_exit) return;

	t = t_entry;

	if (tx_entry > ty_entry)
	{
		ny = 0.0f;
		dx > 0 ? nx = -1.0f : nx = 1.0f;
	}
	else
	{
		nx = 0.0f;
		dy > 0 ? ny = -1.0f : ny = 1.0f;
	}

}

/*
	Extension of original SweptAABB to deal with two moving objects
*/
LPCOLLISIONEVENT CCollision::SweptAABB(LPGAMEOBJECT objSrc, DWORD dt, LPGAMEOBJECT objDest)
{
	float sl, st, sr, sb;		// static object bbox
	float ml, mt, mr, mb;		// moving object bbox
	float t, nx, ny;

	float mvx, mvy;
	objSrc->GetSpeed(mvx, mvy);
	float mdx = mvx * dt;
	float mdy = mvy * dt;

	float svx, svy;
	objDest->GetSpeed(svx, svy);
	float sdx = svx * dt;
	float sdy = svy * dt;

	//
	// NOTE: new m speed = original m speed - collide object speed
	// 
	float dx = mdx - sdx;
	float dy = mdy - sdy;

	objSrc->GetBoundingBox(ml, mt, mr, mb);
	objDest->GetBoundingBox(sl, st, sr, sb);

	SweptAABB(
		ml, mt, mr, mb,
		dx, dy,
		sl, st, sr, sb,
		t, nx, ny,
		objSrc, objDest
	);

	CCollisionEvent* e = new CCollisionEvent(t, nx, ny, dx, dy, objDest, objSrc);
	return e;
}

/*
	Calculate potential collisions with the list of colliable objects

	coObjects: the list of colliable objects
	coEvents: list of potential collisions
*/
void CCollision::Scan(LPGAMEOBJECT objSrc, DWORD dt, vector<LPGAMEOBJECT>* objDests, vector<LPCOLLISIONEVENT>& coEvents)
{
	for (UINT i = 0; i < objDests->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABB(objSrc, dt, objDests->at(i));

		if (e->WasCollided() == 1)
			coEvents.push_back(e);
		else
			delete e;
	}

	//std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CCollision::Filter(LPGAMEOBJECT objSrc,
	vector<LPCOLLISIONEVENT>& coEvents,
	LPCOLLISIONEVENT& colX,
	LPCOLLISIONEVENT& colY,
	int filterBlock = 1,		// 1 = only filter block collisions, 0 = filter all collisions 
	int filterX = 1,			// 1 = process events on X-axis, 0 = skip events on X 
	int filterY = 1)			// 1 = process events on Y-axis, 0 = skip events on Y
{
	float min_tx, min_ty;

	min_tx = 1.0f;
	min_ty = 1.0f;
	int min_ix = -1;
	int min_iy = -1;

	for (UINT i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT c = coEvents[i];
		if (c->isDeleted) continue;
		if (c->obj->IsDeleted()) continue;

		// ignore collision event with object having IsBlocking = 0 (like coin, mushroom, etc)
		if (filterBlock == 1 && !c->obj->IsBlocking())
		{
			continue;
		}

		if (c->t < min_tx && c->nx != 0 && filterX == 1) {
			min_tx = c->t; min_ix = i;
		}

		if (c->t < min_ty && c->ny != 0 && filterY == 1) {
			min_ty = c->t; min_iy = i;
		}
	}

	if (min_ix >= 0) colX = coEvents[min_ix];
	if (min_iy >= 0) colY = coEvents[min_iy];
}

/*
*  Simple/Sample collision framework
*  NOTE: Student might need to improve this based on game logic
*/
void CCollision::Process(LPGAMEOBJECT objSrc, DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vector<LPCOLLISIONEVENT> coEvents;
	LPCOLLISIONEVENT colX = nullptr;
	LPCOLLISIONEVENT colY = nullptr;

	coEvents.clear();

	if (objSrc->IsCollidable())
	{
		Scan(objSrc, dt, coObjects, coEvents);
	}

	// No collision detected
	if (coEvents.size() == 0)
	{
		objSrc->OnNoCollision(dt);
	}
	else
	{
		Filter(objSrc, coEvents, colX, colY);

		float x, y, vx, vy, dx, dy;
		objSrc->GetPosition(x, y);
		objSrc->GetSpeed(vx, vy);
		dx = vx * dt;
		dy = vy * dt;

		if (colX != nullptr && colY != nullptr)
		{
			if (colY->t < colX->t)	// was collision on Y first ?
			{
				y += colY->t * dy + colY->ny * BLOCK_PUSH_FACTOR;
				objSrc->SetPosition(x, y);

				objSrc->OnCollisionWith(colY);

				//
				// see if after correction on Y, is there still a collision on X ? 
				//
				LPCOLLISIONEVENT colX_other = nullptr;

				//
				// check again if there is true collision on X 
				//
				colX->isDeleted = true;		// remove current collision event on X

				// replace with a new collision event using corrected location 
				coEvents.push_back(SweptAABB(objSrc, dt, colX->obj));

				// re-filter on X only
				Filter(objSrc, coEvents, colX_other, colY, /*filterBlock = */ 1, 1, /*filterY=*/0);

				if (colX_other != nullptr)
				{
					x += colX_other->t * dx + colX_other->nx * BLOCK_PUSH_FACTOR;
					objSrc->OnCollisionWith(colX_other);
				}
				else
				{
					x += dx;
				}
			}
			else // collision on X first
			{
				x += colX->t * dx + colX->nx * BLOCK_PUSH_FACTOR;
				objSrc->SetPosition(x, y);

				objSrc->OnCollisionWith(colX);

				//
				// see if after correction on X, is there still a collision on Y ? 
				//
				LPCOLLISIONEVENT colY_other = nullptr;

				//
				// check again if there is true collision on Y
				//
				colY->isDeleted = true;		// remove current collision event on Y

				// replace with a new collision event using corrected location 
				coEvents.push_back(SweptAABB(objSrc, dt, colY->obj));

				// re-filter on Y only
				Filter(objSrc, coEvents, colX, colY_other, /*filterBlock = */ 1, /*filterX=*/0, /*filterY=*/1);

				if (colY_other != nullptr)
				{
					y += colY_other->t * dy + colY_other->ny * BLOCK_PUSH_FACTOR;
					objSrc->OnCollisionWith(colY_other);
				}
				else
				{
					y += dy;
				}
			}
		}
		else
			if (colX != nullptr)
			{
				x += colX->t * dx + colX->nx * BLOCK_PUSH_FACTOR;
				y += dy;
				objSrc->OnCollisionWith(colX);
			}
			else
				if (colY != nullptr)
				{
					x += dx;
					y += colY->t * dy + colY->ny * BLOCK_PUSH_FACTOR;

					objSrc->OnCollisionWith(colY);
				}
				else // both colX & colY are nullptr 
				{
					x += dx;
					y += dy;
				}
		objSrc->SetPosition(x, y);
	}

	//
	// Scan all non-blocking collisions for further collision logic
	//
	for (UINT i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT e = coEvents[i];
		if (e->isDeleted) continue;
		if (CGoomba* g = dynamic_cast<CGoomba*>(e->obj))
			if (g->GetIsDefeated() == true)
				continue;
		else if (CPiranhaPlant* p = dynamic_cast<CPiranhaPlant*>(e->obj))
			if (p->GetIsDefeated() == true)
				continue;
		else if (CKoopa* k = dynamic_cast<CKoopa*>(e->obj))
			if (k->GetIsDefeated() == true)
				continue;
		else if (CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj))
			if (wg->GetIsDefeated() == true)
				continue;

		if ((dynamic_cast<CCoinQBlock*>(e->obj) || dynamic_cast<CBuffQBlock*>(e->obj))
			&& dynamic_cast<CTailWhip*>(objSrc))
		{
			objSrc->OnCollisionWith(e); // tail whip can hit buffqblock
			continue;
		}
		if ((dynamic_cast<CWingedGoomba*>(e->obj) || dynamic_cast<CGoomba*>(e->obj))
			&& dynamic_cast<CKoopa*>(objSrc))
		{
			objSrc->OnCollisionWith(e); // Koopa can hit WingedGoomba and Goomba
			continue;
		}
		//if (e->obj->IsBlocking() && !dynamic_cast<CBuffQBlock*>(e->obj)) continue;  // blocking collisions were handled already, skip them
		if (e->obj->IsBlocking()) continue;  // blocking collisions were handled already, skip them

		objSrc->OnCollisionWith(e);
	}


	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}
