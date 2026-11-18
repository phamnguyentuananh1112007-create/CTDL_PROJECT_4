#pragma once

#include "HangHoa.h"
#include <vector>
#include <utility>

class HangHoaSapXep
{
public:
    /**************************************************************************
     * @Description Sap xep danh sach hang hoa theo gia bang Bubble Sort
     *
     * @param       danhSach: Danh sach hang hoa can sap xep
     * @param       bTangDan: true - tang dan, false - giam dan
     * @return      None
     *************************************************************************/
    static void bubbleSort(std::vector<HangHoa>& danhSach, const bool bTangDan = true);
};