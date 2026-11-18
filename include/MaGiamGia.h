#pragma once
#include <string>
#include <iosfwd>

const int COL_WIDTH_MA_GG = 14;
const int COL_WIDTH_MA_HH_GG = 12;
const int COL_WIDTH_SL_TOI_DA = 22;
const int COL_WIDTH_PHAN_TRAM = 16;
const int COL_WIDTH_SL_CON_LAI = 15;
const double MAX_DISCOUNT_PERCENT = 100.0;

class MaGiamGia
{
private:
    std::string _strMaGiamGia;
    std::string _strMaHang;
    int _iSoLuongToiDa;    
    double _dPhanTramGiam; 
    int _iSoLuongConLai;    

public:
    /**************************************************************************
     * @Description Khoi tao mac dinh cho doi tuong MaGiamGia
     *
     * @return      None
     * @attention   Tat ca bien thanh vien duoc khoi tao gia tri an toan
     *************************************************************************/
    MaGiamGia();

    /**************************************************************************
     * @Description Khoi tao co tham so cho doi tuong MaGiamGia
     *
     * @return      None
     * @attention   Truyen day du 5 thuoc tinh cua ma giam gia
     *************************************************************************/
    MaGiamGia(const std::string& strMaGG, const std::string& strMaHH, const int iSlToiDa, const double dPtGiam, const int iSlConLai);

    /**************************************************************************
     * @Description Lay ma code giam gia
     *
     * @return      Chuoi ma giam gia
     * @attention   Ham hang khong thay doi bien thanh vien
     *************************************************************************/
    std::string getMaGiamGia() const;

    /**************************************************************************
     * @Description Lay ma hang hoa duoc ap dung giam gia
     *
     * @return      Chuoi ma hang hoa
     * @attention   Ham hang khong thay doi bien thanh vien
     *************************************************************************/
    std::string getMaHang() const;

    /**************************************************************************
     * @Description Lay so luong ma giam gia toi da cho phep tren 1 mat hang
     *
     * @return      So nguyen duong chi gioi han su dung
     * @attention   Ham hang khong thay doi bien thanh vien
     *************************************************************************/
    int getSoLuongToiDa() const;

    /**************************************************************************
     * @Description Lay phan tram giam gia tren don gia ban dau
     *
     * @return      Gia tri phan tram giam (0 < dPhanTram <= 100)
     * @attention   Ham hang khong thay doi bien thanh vien
     *************************************************************************/
    double getPhanTramGiam() const;

    /**************************************************************************
     * @Description Lay so luong ma giam gia con lai trong he thong
     *
     * @return      So luong ma giam gia con lai
     * @attention   Ham hang khong thay doi bien thanh vien
     *************************************************************************/
    int getSoLuongConLai() const;

    /**************************************************************************
     * @Description Cap nhat so luong ma giam gia con lai
     *
     * @return      true neu cap nhat hop le, false neu ngoai gioi han
     * @attention   Duoc goi sau khi khach hang ap ma hoac Admin cap nhat
     *************************************************************************/
    bool setSoLuongConLai(const int iSoLuongMoi);

    /**************************************************************************
     * @Description Nhap thong tin chi tiet cho ma giam gia tu ban phim
     *
     * @return      true neu nhap day du, false neu dau vao ket thuc som
     * @attention   Kiem tra rang buoc mien gia tri cho tung thuoc tinh
     *************************************************************************/
    bool nhap(const std::string& strMaGG, const std::string& strMaHH);

    /**************************************************************************
     * @Description Xuat thong tin ma giam gia thanh mot dong trong bang
     *
     * @return      None
     * @attention   Can in tieu de bang truoc khi goi ham nay
     *************************************************************************/
    void xuat() const;

    /**************************************************************************
     * @Description Ghi thong tin ma giam gia ra luong file
     *
     * @return      None
     * @attention   Dinh dang: MaGiamGia|MaHang|SoLuongToiDa|PhanTramGiam|SoLuongConLai
     *************************************************************************/
    void ghiFile(std::ofstream& fileOut) const;

    /**************************************************************************
     * @Description Doc thong tin mot ma giam gia tu luong file
     *
     * @return      true neu doc thanh cong, false neu het file hoac du lieu loi
     * @attention   Doc mot dong va kiem tra dung 5 truong phan cach bang '|'
     *************************************************************************/
    bool docFile(std::ifstream& fileIn);

    static bool docDong(const std::string& strDong, MaGiamGia& maGiamGia);
};