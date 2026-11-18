#include "HangHoaSapXep.h"

void HangHoaSapXep::bubbleSort(std::vector<HangHoa>& danhSach, const bool bTangDan)
{
    for (std::vector<HangHoa>::size_type i = 0; i < danhSach.size(); ++i)
    {
        bool bHoanDoi = false;

        for (std::vector<HangHoa>::size_type j = 0;
             j < danhSach.size() - i - 1; ++j)
        {
            const double dGia1 = danhSach[j].getGiaBan();
            const double dGia2 = danhSach[j + 1].getGiaBan();

            const bool bCanDoi = bTangDan
                ? dGia1 > dGia2
                : dGia1 < dGia2;

            if (bCanDoi)
            {
                std::swap(danhSach[j], danhSach[j + 1]);
                bHoanDoi = true;
            }
        }

        if (!bHoanDoi)
        {
            break;
        }
    }
}
