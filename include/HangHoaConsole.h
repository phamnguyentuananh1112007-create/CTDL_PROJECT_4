#pragma once

#include "DanhSachHangHoa.h"
#include "HangHoaSapXep.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <string>

class HangHoaConsole
{
private:
    enum
    {
        COL_WIDTH_MA = 7,
        COL_WIDTH_TEN = 32,
        COL_WIDTH_NOI_SX = 15,
        COL_WIDTH_MAU = 12,
        COL_WIDTH_GIA = 14,
        COL_WIDTH_NGAY = 14,
        COL_WIDTH_SL = 9
    };

    std::istream& _input;
    std::ostream& _output;

    static std::string trim(const std::string& strInput)
    {
        const std::string::size_type iDau = strInput.find_first_not_of(" \t\r\n");
        if (iDau == std::string::npos)
        {
            return std::string();
        }

        const std::string::size_type iCuoi = strInput.find_last_not_of(" \t\r\n");
        return strInput.substr(iDau, iCuoi - iDau + 1);
    }

    static std::string toLowerString(const std::string& strInput);
    static bool ngayHopLe(const std::string& strNgay);

    static bool chuyenChuoiSangSoNguyen(const std::string& strInput, int& iKetQua);

    static bool chuyenChuoiSangSoThuc(const std::string& strInput, double& dKetQua);

    bool docDong(const std::string& strThongBao, std::string& strKetQua) const;

    bool docChuoiBatBuoc(const std::string& strThongBao, std::string& strKetQua) const;

    bool docGiaBan(double& dGiaBan) const;
    bool docNgayNhapKho(std::string& strNgay) const;
    bool docSoLuong(int& iSoLuong) const;

    void inTieuDeBang() const;

    void inMotHangHoa(const HangHoa& hangHoa) const;

public:
    explicit HangHoaConsole(
        std::istream& input = std::cin,
        std::ostream& output = std::cout
    );

    /**************************************************************************
     * @Description Hien thi danh sach hang hoa; co the chi hien thi hang dang ban
     *************************************************************************/
    void hienThi(const DanhSachHangHoa& danhSach, const bool chiHangDangBan = false) const;

    /**************************************************************************
     * @Description Sap xep va hien thi danh sach hang hoa theo gia ban.
     *              true: gia tu thap den cao; false: gia tu cao den thap.
     *              Danh sach goc khong bi thay doi.
     *************************************************************************/
    void hienThiSapXep(
        const DanhSachHangHoa& danhSach,
        const bool bTangDan,
        const bool chiHangDangBan = false
    ) const;

    /**************************************************************************
     * @Description Tim kiem ten hang; chi tra ve hang dang ban co ton kho > 0
     *************************************************************************/
    void timKiem(const DanhSachHangHoa& danhSach) const;

    /**************************************************************************
     * @Description Nhap va them hang hoa moi; cho phep nhap lai ma khong hop le
     * @return      true neu danh sach da thay doi
     *************************************************************************/
    bool themHangHoa(DanhSachHangHoa& danhSach) const;

    /**************************************************************************
     * @Description Xoa hang hoa; yeu cau xac nhan truoc khi xoa
     * @return      true neu danh sach da thay doi
     *************************************************************************/
    bool xoaHangHoa(
        DanhSachHangHoa& danhSach,
        const std::function<bool(const std::string&)>& kiemTraDangCho = std::function<bool(const std::string&)>()
    ) const;

    /**************************************************************************
     * @Description Cap nhat so luong; nhap sai se duoc yeu cau nhap lai
     * @return      true neu so luong da thay doi
     *************************************************************************/
    bool capNhatSoLuong(DanhSachHangHoa& danhSach) const;
};