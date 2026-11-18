#pragma once

#include "HangHoa.h"
#include "Node.h"
#include <cstddef>
#include <cctype>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

typedef Node<HangHoa> NODE;
typedef NODE* PNODE;

class DanhSachHangHoa
{
private:
    enum { MA_HANG_REQUIRED_LENGTH = 4 };

    LinkedList<HangHoa> _hangHoa;
    std::unordered_set<std::string> _setMaHang;

    /**************************************************************************
     * @Description Cat bo khoang trang o dau va cuoi chuoi
     *************************************************************************/
    static std::string trim(const std::string& strInput);

    /**************************************************************************
     * @Description Giai phong toan bo node trong danh sach
     *************************************************************************/
public:
    /**************************************************************************
     * @Description Khoi tao danh sach lien ket don rong
     *************************************************************************/
    DanhSachHangHoa() noexcept;

    /**************************************************************************
     * @Description Copy constructor - sao chep sau noi dung danh sach
     *************************************************************************/
    DanhSachHangHoa(const DanhSachHangHoa& danhSachKhac);

    /**************************************************************************
     * @Description Gan sao chep an toan theo copy-and-swap
     *************************************************************************/
    DanhSachHangHoa& operator=(const DanhSachHangHoa& danhSachKhac);

    /**************************************************************************
     * @Description Move constructor
     *************************************************************************/
    DanhSachHangHoa(DanhSachHangHoa&& danhSachKhac) noexcept;

    /**************************************************************************
     * @Description Move assignment
     *************************************************************************/
    DanhSachHangHoa& operator=(DanhSachHangHoa&& danhSachKhac) noexcept;

    /**************************************************************************
     * @Description Huy danh sach lien ket don va giai phong bo nho
     *************************************************************************/
    ~DanhSachHangHoa();

    /**************************************************************************
     * @Description Hoan doi noi dung hai danh sach
     *************************************************************************/
    void swap(DanhSachHangHoa& danhSachKhac) noexcept;

    /**************************************************************************
     * @Description Chuan hoa ma hang: cat khoang trang va viet hoa
     *************************************************************************/
    static std::string chuanHoaMaHang(const std::string& strMa);

    /**************************************************************************
     * @Description Lay do dai ma hang bat buoc
     *************************************************************************/
    static std::string::size_type doDaiMaHangBatBuoc() noexcept;

    /**************************************************************************
     * @Description Kiem tra ma hang co dung dinh dang hay khong
     *************************************************************************/
    static bool maHangHopLe(const std::string& strMa);

    /**************************************************************************
     * @Description Kiem tra danh sach rong
     *************************************************************************/
    bool empty() const noexcept;

    /**************************************************************************
     * @Description Lay node dau danh sach chi doc
     *************************************************************************/
    const NODE* getHead() const noexcept;

    /**************************************************************************
     * @Description Kiem tra ma hang hoa da ton tai
     *************************************************************************/
    bool kiemTraMa(const std::string& strMa) const;

    /**************************************************************************
     * @Description Lay thong tin HangHoa theo ma hang
     *************************************************************************/
    bool layHangHoaTheoMa(const std::string& strMa, HangHoa& ketQua) const;

    /**************************************************************************
     * @Description Cap nhat so luong ton kho theo ma hang
     *************************************************************************/
    bool capNhatSoLuongTheoMa(const std::string& strMa, const int iSoLuongMoi);

    /**************************************************************************
     * @Description Them hang hoa vao cuoi danh sach template trong O(1)
     * @return      false neu ma khong hop le hoac bi trung
     * @attention   LinkedList giu tail; bang bam ma hang kiem tra trung O(1) trung binh
     *************************************************************************/
    bool them(const HangHoa& hangHoa);

    /**************************************************************************
     * @Description Xoa hang hoa theo ma va cap nhat tail cua danh sach template
     *************************************************************************/
    bool xoaTheoMa(const std::string& strMa);
};