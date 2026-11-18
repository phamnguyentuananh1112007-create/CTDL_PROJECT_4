#include "MaGiamGia.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

namespace
{
    string trimField(const string& value)
    {
        const string::size_type first = value.find_first_not_of(" \t\r\n");
        if (first == string::npos)
        {
            return string();
        }
        const string::size_type last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, last - first + 1);
    }

    bool parseInteger(const string& value, int& result)
    {
        istringstream input(value);
        long long parsed = 0;
        if (!(input >> parsed))
        {
            return false;
        }
        input >> ws;
        if (!input.eof() || parsed < (numeric_limits<int>::min)() ||
            parsed > (numeric_limits<int>::max)())
        {
            return false;
        }
        result = static_cast<int>(parsed);
        return true;
    }

    bool parseDouble(const string& value, double& result)
    {
        istringstream input(value);
        double parsed = 0.0;
        if (!(input >> parsed))
        {
            return false;
        }
        input >> ws;
        if (!input.eof() || !isfinite(parsed))
        {
            return false;
        }
        result = parsed;
        return true;
    }
}

MaGiamGia::MaGiamGia()
{
    this->_strMaGiamGia = "";
    this->_strMaHang = "";
    this->_iSoLuongToiDa = 0;
    this->_dPhanTramGiam = 0.0;
    this->_iSoLuongConLai = 0;
}

MaGiamGia::MaGiamGia(const string& strMaGG, const string& strMaHH, const int iSlToiDa, const double dPtGiam, const int iSlConLai)
{
    this->_strMaGiamGia = strMaGG;
    this->_strMaHang = strMaHH;
    this->_iSoLuongToiDa = iSlToiDa;
    this->_dPhanTramGiam = dPtGiam;
    this->_iSoLuongConLai = iSlConLai;
}

string MaGiamGia::getMaGiamGia() const
{
    return this->_strMaGiamGia;
}

string MaGiamGia::getMaHang() const
{
    return this->_strMaHang;
}

int MaGiamGia::getSoLuongToiDa() const
{
    return this->_iSoLuongToiDa;
}

double MaGiamGia::getPhanTramGiam() const
{
    return this->_dPhanTramGiam;
}

int MaGiamGia::getSoLuongConLai() const
{
    return this->_iSoLuongConLai;
}

bool MaGiamGia::setSoLuongConLai(const int iSoLuongMoi)
{
    if (iSoLuongMoi < 0 || iSoLuongMoi > this->_iSoLuongToiDa)
    {
        return false;
    }
    this->_iSoLuongConLai = iSoLuongMoi;
    return true;
}

bool MaGiamGia::nhap(const string& strMaGG, const string& strMaHH)
{
    this->_strMaGiamGia = strMaGG;
    this->_strMaHang = strMaHH;

    while (true)
    {
        cout << "So luong ma toi da co the dung cho 1 mat hang: ";
        string strNhap;
        if (!getline(cin, strNhap))
        {
            return false;
        }
        if (parseInteger(strNhap, this->_iSoLuongToiDa) &&
            this->_iSoLuongToiDa > 0)
        {
            break;
        }
        cout << "Gia tri khong hop le! Vui long nhap so nguyen > 0." << endl;
    }

    while (true)
    {
        cout << "Phan tram giam gia (%, tu 1 den 100): ";
        string strNhap;
        if (!getline(cin, strNhap))
        {
            return false;
        }
        if (parseDouble(strNhap, this->_dPhanTramGiam) &&
            this->_dPhanTramGiam > 0.0 &&
            this->_dPhanTramGiam <= MAX_DISCOUNT_PERCENT)
        {
            break;
        }
        cout << "Phan tram giam phai trong khoang (0, 100]!" << endl;
    }

    while (true)
    {
        cout << "So luong ma giam gia con lai: ";
        string strNhap;
        if (!getline(cin, strNhap))
        {
            return false;
        }
        if (parseInteger(strNhap, this->_iSoLuongConLai) &&
            this->_iSoLuongConLai >= 0 &&
            this->_iSoLuongConLai <= this->_iSoLuongToiDa)
        {
            return true;
        }
        cout << "So luong con lai phai tu 0 den so luong toi da ("
             << this->_iSoLuongToiDa << ")!" << endl;
    }
}

void MaGiamGia::xuat() const
{
    cout << left
         << setw(COL_WIDTH_MA_GG) << this->_strMaGiamGia
         << setw(COL_WIDTH_MA_HH_GG) << this->_strMaHang
         << setw(COL_WIDTH_SL_TOI_DA) << this->_iSoLuongToiDa
         << setw(COL_WIDTH_PHAN_TRAM) << fixed << setprecision(1) << this->_dPhanTramGiam
         << setw(COL_WIDTH_SL_CON_LAI) << this->_iSoLuongConLai << endl;
}

void MaGiamGia::ghiFile(ofstream& fileOut) const
{
    fileOut << this->_strMaGiamGia << "|"
            << this->_strMaHang << "|"
            << this->_iSoLuongToiDa << "|"
            << this->_dPhanTramGiam << "|"
            << this->_iSoLuongConLai << endl;
}

bool MaGiamGia::docFile(ifstream& fileIn)
{
    string strDong;
    if (!getline(fileIn, strDong))
    {
        return false;
    }

    return MaGiamGia::docDong(strDong, *this);
}

bool MaGiamGia::docDong(const string& strDong, MaGiamGia& maGiamGia)
{
    string truong[5];
    string::size_type viTriDau = 0;
    for (int i = 0; i < 4; ++i)
    {
        const string::size_type viTriPhanCach = strDong.find('|', viTriDau);
        if (viTriPhanCach == string::npos)
        {
            return false;
        }
        truong[i] = trimField(strDong.substr(viTriDau, viTriPhanCach - viTriDau));
        viTriDau = viTriPhanCach + 1;
    }
    if (strDong.find('|', viTriDau) != string::npos)
    {
        return false;
    }
    truong[4] = trimField(strDong.substr(viTriDau));

    int iSoLuongToiDa = 0;
    int iSoLuongConLai = 0;
    double dPhanTramGiam = 0.0;
    if (truong[0].empty() || truong[1].empty() ||
        !parseInteger(truong[2], iSoLuongToiDa) || iSoLuongToiDa <= 0 ||
        !parseDouble(truong[3], dPhanTramGiam) ||
        dPhanTramGiam <= 0.0 || dPhanTramGiam > MAX_DISCOUNT_PERCENT ||
        !parseInteger(truong[4], iSoLuongConLai) || iSoLuongConLai < 0 ||
        iSoLuongConLai > iSoLuongToiDa)
    {
        return false;
    }

    maGiamGia = MaGiamGia(
        truong[0], truong[1], iSoLuongToiDa, dPhanTramGiam, iSoLuongConLai);
    return true;
}
