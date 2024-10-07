#include "upravl_po_ekr.h"

upravl_perem_po_ekr_el::upravl_perem_po_ekr_el() {
    mPosX = mPosY = mVelX = mVelY = 0;
}

void upravl_perem_po_ekr_el::setPos(int x, int y){
    mPosX = x;
    mPosY = y;
}

void upravl_perem_po_ekr_el::setVel(int x, int y){
    mVelX = x;
    mVelY = y;
}

int upravl_perem_po_ekr_el::GetY(){
    return mPosY;
}

int upravl_perem_po_ekr_el::GetX(){
    return mPosX;
}
