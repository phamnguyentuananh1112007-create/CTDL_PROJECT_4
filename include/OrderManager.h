#pragma once

#include "DanhSachHangHoa.h"
#include "DanhSachMaGiamGia.h"
#include "HangHoaRepository.h"
#include "Queue.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <limits>
#include <cstdio>
#include <cmath>
#include <unordered_set>


const char* const FILE_DON_HANG = "data/DonHang.txt";
const char* const TRANG_THAI_CHO_XU_LY = "CHO_XU_LY";
const char* const TRANG_THAI_DA_XU_LY = "DA_XU_LY";
const char* const TRANG_THAI_DA_HUY = "DA_HUY";

// Chi tiet mot mat hang trong don hang.
struct ChiTietDonHang
{
    std::string maHang;
    int soLuong;
    double donGia;

    ChiTietDonHang();

    ChiTietDonHang(const std::string& strMaHang, const int iSoLuong, const double dDonGia);
};

// Thong tin mot don hang. Vector _dsDonHang ben duoi duoc giu theo thu tu doc/tao.
struct DonHang
{
    int maDonHang;
    std::string trangThai;
    std::string tenKhachHang;
    std::string taiKhoan;
    std::string diaChi;
    std::string soDienThoai;
    std::string ngayDat; // Luu theo dinh dang dd/MM/yyyy HH:mm:ss
    double tongTien;
    std::vector<ChiTietDonHang> chiTiet;
    // true neu so luong san pham cua don da duoc giu/tru kho.
    // Duoc luu trong DonHang.txt de tranh tru kho lap lai khi khoi dong.
    bool tonKhoDaTru;

    DonHang();
};

/******************************************************************************
 * @Description Quan ly don hang theo nguyen tac FIFO (First In - First Out)
 * @attention   Don moi duoc them vao cuoi danh sach; Admin chi xu ly don cho
 *              xu ly dau tien. Neu don dau tien khong du hang, khong bo qua
 *              don do de xu ly don phia sau.
 ******************************************************************************/
class OrderManager
{
private:
    DanhSachHangHoa& _dsHangHoa;
    DanhSachMaGiamGia& _dsMaGiamGia;
    HangHoaRepository& _repositoryHangHoa;
    std::vector<DonHang> _dsDonHang;
    std::vector<std::string> _dsDongDonKhongHopLe;
    Queue<DonHang> _qDonChoXuLy;
    int _iMaDonTiepTheo;
    std::string _strMaGiamGiaChoDonTiepTheo;

    static std::string trim(const std::string& strInput);

    static bool coKyTuPhanCach(const std::string& strInput);

    static bool ngayHopLe(const std::string& strNgay);

    static bool ngayKhongTuongLai(const std::string& strNgay);

    static bool truongBatBuocHopLe(const std::string& strInput);

    static bool maHangFileHopLe(const std::string& strMaHang);

    static bool tepTonTai(const std::string& strTenTep);

    int timViTriDonTheoMa(const int iMaDon) const;

    // Dong bo lai hang doi FIFO tu _dsDonHang (loai bo cac don da huy / da xu ly)
    void taiTaoHangDoiChoXuLy();

    static bool chuyenChuoiSangSoNguyen(const std::string& strInput, int& iKetQua);

    static bool chuyenChuoiSangSoThuc(const std::string& strInput, double& dKetQua);

    bool nhapSoNguyen(const std::string& strThongBao, const int iGiaTriNhoNhat, int& iKetQua) const;

    bool nhapChuoiBatBuoc(const std::string& strThongBao, std::string& strKetQua) const;

    // Lay thoi gian hien tai day du: dd/MM/yyyy HH:mm:ss
    static std::string layNgayHienTai();

    static std::string dinhDangThoiGianHienThi(const std::string& strNgayDat);

    int timDonChoXuLyDauTien() const;

    // Chi tinh so luong cua cac don CHO_XU_LY cu chua duoc tru kho (neu con).
    long long tinhSoLuongChoChuaTru(const std::string& strMaHang) const;

    static bool hopNhatChiTiet(std::vector<ChiTietDonHang>& dsChiTiet,
                               const ChiTietDonHang& chiTietMoi);

    bool docChiTietDonHang(const std::string& strDuLieu, std::vector<ChiTietDonHang>& dsChiTiet) const;

    bool docMotDonHang(const std::string& strDong, DonHang& donHang) const;

    void inThongTinDonHang(const DonHang& donHang) const;

    bool kiemTraDuHang(const DonHang& donHang, std::string& strLyDo) const;

    // Thay doi ton kho theo chi tiet don: bTruKho=true de giu hang, false de hoan hang.
    // Ham tu rollback danh sach neu mot mat hang khong the cap nhat.
    bool thayDoiTonKhoTheoDon(const DonHang& donHang, const bool bTruKho, std::string& strLyDo);

    bool thucHienHuyDonTheoMa(const int iMaDon, const std::string& strTaiKhoanKiemTra);

public:
    OrderManager(
        DanhSachHangHoa& dsHangHoa,
        DanhSachMaGiamGia& dsMaGiamGia,
        HangHoaRepository& repositoryHangHoa
    );

    void docFile();

    bool ghiFile() const;

    /**************************************************************************
     * @Description Dat hang: tao don moi o cuoi hang doi, trang thai CHO_XU_LY.
     *************************************************************************/
    void datHang(const std::string& strTaiKhoan);

    void menuApMaGiamGia();

    bool hienThiDonHangCuaTaiKhoan(const std::string& strTaiKhoan, const bool bChiDonChoXuLy = false) const;

    void huyDonHangKhachHang(const std::string& strTaiKhoan);

    void huyDonHangAdmin();

    void hienThiHangCho() const;

    void hienThiTatCaDonHang() const;

    void xuLyDonTiepTheo();

    bool coDonChoTheoMaHang(const std::string& strMaHang) const;

    void menuQuanLyDonHang();
};
