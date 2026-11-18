#pragma once
#include "MaGiamGia.h"
#include "Node.h"
#include "DanhSachHangHoa.h"

const char* const FILE_MA_GIAM_GIA = "data/MaGiamGia.txt";

class DanhSachMaGiamGia
{
private:
    LinkedList<MaGiamGia> _danhSach;

    /**************************************************************************
     * @Description Cat bo khoang trang o dau va cuoi chuoi
     *
     * @return      Chuoi sau khi da loai bo khoang trang thua
     * @attention   Tra ve chuoi rong neu chuoi chi chua khoang trang
     *************************************************************************/
    static std::string trim(const std::string& strInput);

    /**************************************************************************
     * @Description In tieu de bang danh sach ma giam gia
     *
     * @return      None
     * @attention   Su dung cung do rong cot voi MaGiamGia::xuat()
     *************************************************************************/
    void inTieuDeBang() const;

public:
    DanhSachMaGiamGia() = default;
    ~DanhSachMaGiamGia() = default;

    /**************************************************************************
     * @Description Kiem tra ma giam gia da ton tai trong danh sach hay chua
     *
     * @return      true neu ton tai, false neu chua ton tai
     * @attention   Ham hang khong thay doi du lieu thanh vien
     *************************************************************************/
    bool kiemTraMa(const std::string& strMaGG) const;

    /**************************************************************************
     * @Description Lay thong tin doi tuong MaGiamGia theo ma code
     *
     * @return      true neu tim thay, false neu khong tim thay
     * @attention   Ket qua duoc gan vao tham chieu ketQua
     *************************************************************************/
    bool layMaGiamGiaTheoMa(const std::string& strMaGG, MaGiamGia& ketQua) const;

    /**************************************************************************
     * @Description Cap nhat so luong con lai cua mot ma giam gia
     *
     * @return      true neu cap nhat thanh cong, false neu khong tim thay
     * @attention   Dung sau khi khach hang ap dung ma thanh cong
     *************************************************************************/
    bool capNhatSoLuongConLaiTheoMa(const std::string& strMaGG, const int iSoLuongMoi);

    /**************************************************************************
     * @Description Them ma giam gia vao cuoi danh sach template
     *
     * @return      true neu them thanh cong, false neu du lieu loi hoac trung
     * @attention   Bo nho node do LinkedList<MaGiamGia> quan ly
     *************************************************************************/
    bool them(const MaGiamGia& maGiamGia);

    /**************************************************************************
     * @Description Hien thi toan bo danh sach ma giam gia ra man hinh
     *
     * @return      None
     * @attention   Thong bao neu danh sach dang rong
     *************************************************************************/
    void hienThi() const;

    /**************************************************************************
     * @Description Hien thi cac ma giam gia con luot dung cua mot ma hang cu the
     *
     * @return      true neu co it nhat 1 ma kha dung, false neu khong co
     * @attention   Chi hien thi cac ma co SoLuongConLai > 0
     *************************************************************************/
    bool hienThiTheoMaHang(const std::string& strMaHang) const;

    /**************************************************************************
     * @Description Nhap va them mot ma giam gia moi tu ban phim
     *
     * @return      true neu them thanh cong, false neu bi huy hoac loi
     * @attention   Ma hang ap dung phai ton tai trong DanhSachHangHoa
     *************************************************************************/
    bool themMaGiamGia(const DanhSachHangHoa& dsHangHoa);

    /**************************************************************************
     * @Description Cap nhat so luong con lai cho ma giam gia tu ban phim
     *
     * @return      true neu cap nhat thanh cong
     * @attention   So luong moi phai la so nguyen >= 0
     *************************************************************************/
    bool capNhatSoLuong();

    /**************************************************************************
     * @Description Xoa mot ma giam gia khoi danh sach theo ma code
     *
     * @return      true neu xoa thanh cong
     * @attention   LinkedList tu giai phong node bi xoa
     *************************************************************************/
    bool xoaMaGiamGia();

    /**************************************************************************
     * @Description Doc tung dong ma giam gia va bo qua ban ghi loi
     *
     * @return      None
     * @attention   Chi nap ma giam gia tham chieu den ma hang dang ton tai
     *************************************************************************/
    void docFile(const DanhSachHangHoa& dsHangHoa);

    /**************************************************************************
     * @Description Ghi toan bo danh sach ma giam gia ra file MaGiamGia.txt
     *
     * @return      true neu ghi tep thanh cong
     * @attention   Dung tep tam va backup truoc khi thay tep chinh
     *************************************************************************/
    bool ghiFile() const;
};