#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>
#include <cctype>

class HangHoa
{
private:
    std::string _strMaHang;
    std::string _strTenHang;
    std::string _strNoiSanXuat;
    std::string _strMauSac;
    double _dGiaBan;
    std::string _strNgayNhapKho;
    int _iSoLuong;

    static bool chuoiGiaHopLe(const std::string& s, double& gia);

    static bool chuoiSoLuongHopLe(const std::string& s, int& sl);

public:
    HangHoa();

    HangHoa(
        const std::string& ma,
        const std::string& ten,
        const std::string& noiSX,
        const std::string& mau,
        double gia,
        const std::string& ngay,
        int soLuong
    );

    // ===== CAC GETTER THEO CODE MOI =====
    static bool ngayHopLe(const std::string& ngay);

    // Kiem tra du lieu quan trong truoc khi dua hang hoa vao danh sach.
    // Gia ban phai la so huu han va lon hon 0; ngay phai ton tai tren lich.
    bool duLieuHopLe() const;

    const std::string& getMaHang() const;

    const std::string& getTenHang() const;

    const std::string& getNoiSanXuat() const;

    const std::string& getMauSac() const;

    double getGiaBan() const;

    const std::string& getNgayNhapKho() const;

    int getSoLuong() const;

    // ===== CAC HAM TUONG THICH VOI CODE CU =====

    std::string getmaHang() const;

    std::string gettenHang() const;

    double getgiaBan() const;

    int getsoLuong() const;

    void setMaHang(const std::string& ma);

    void setSoLuong(int sl);

    void setsoLuong(int sl);

    // ===== NHAP HANG HOA =====

    bool Nhap(const std::string& ma);

    // ===== XUAT HANG HOA =====

    void Xuat() const;

    // ===== GHI FILE =====

    void ghiFile(std::ofstream& file) const;

    // ===== DOC FILE =====

    bool docFile(std::ifstream& file);
};
