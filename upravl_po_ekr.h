#ifndef UPRAVL_PO_EKR_H
#define UPRAVL_PO_EKR_H

class upravl_perem_po_ekr_el
{
     protected:

		int mPosX, mPosY;

		int mVelX, mVelY;

     public:

		upravl_perem_po_ekr_el();

		virtual void move() = 0;

		void setPos(int x, int y);

		int GetX();

		int GetY();

		void setVel(int x, int y);
};

#endif // UPRAVL_PO_EKR_H
