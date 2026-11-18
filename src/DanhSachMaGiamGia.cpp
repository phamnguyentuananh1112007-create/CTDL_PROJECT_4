#include "DanhSachMaGiamGia.h"
#include "NodeMaGiamGia.h"

#include <windows.h>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

string DanhSachMaGiamGia::trim(const string& strInput)
{
    size_t iDau = strInput.find_first_not_of(" \t\r\n");
    if (iDau == string::npos)
    {
        return "";
    }
    size_t iCuoi = strInput.find_last_not_of(" \t\r\n");
    return strInput.substr(iDau, iCuoi - iDau + 1);
}

void DanhSachMaGiamGia::inTieuDeBang() const
{
    cout << left
         << setw(COL_WIDTH_MA_GG) << "Ma Giam Gia"
         << setw(COL_WIDTH_MA_HH_GG) << "Ma Hang"
         << setw(COL_WIDTH_SL_TOI_DA) << "SL Toi Da/Mat Hang"
         << setw(COL_WIDTH_PHAN_TRAM) << "Giam (%)"
         << setw(COL_WIDTH_SL_CON_LAI) << "SL Con Lai" << endl;
    cout << "-------------------------------------------------------------------------------" << endl;
}

bool DanhSachMaGiamGia::kiemTraMa(const string& strMaGG) const
{
    const NODEMAGIAMGIA* pCurrent = this->_danhSach.head();
    while (pCurrent != nullptr)
    {
        if (pCurrent->Data.getMaGiamGia() == strMaGG)
        {
            return true;
        }
        pCurrent = pCurrent->Next;
    }
    return false;
}

bool DanhSachMaGiamGia::layMaGiamGiaTheoMa(const string& strMaGG, MaGiamGia& ketQua) const
{
    const NODEMAGIAMGIA* pCurrent = this->_danhSach.head();
    while (pCurrent != nullptr)
    {
        if (pCurrent->Data.getMaGiamGia() == strMaGG)
        {
            ketQua = pCurrent->Data;
            return true;
        }
        pCurrent = pCurrent->Next;
    }
    return false;
}

bool DanhSachMaGiamGia::capNhatSoLuongConLaiTheoMa(const string& strMaGG, const int iSoLuongMoi)
{
    if (iSoLuongMoi < 0)
    {
        return false;
    }

    PNODEMAGIAMGIA pCurrent = this->_danhSach.head();
    while (pCurrent != nullptr)
    {
        if (pCurrent->Data.getMaGiamGia() == strMaGG)
        {
            return pCurrent->Data.setSoLuongConLai(iSoLuongMoi);
        }
        pCurrent = pCurrent->Next;
    }
    return false;
}

bool DanhSachMaGiamGia::them(const MaGiamGia& maGiamGia)
{
    const string strMaGG = maGiamGia.getMaGiamGia();
    const string strMaHang = maGiamGia.getMaHang();
    if (strMaGG.empty() ||
        strMaGG.find_first_of("|\r\n") != string::npos ||
        !DanhSachHangHoa::maHangHopLe(strMaHang) ||
        strMaHang.find_first_of("|\r\n") != string::npos ||
        maGiamGia.getSoLuongToiDa() <= 0 ||
        !isfinite(maGiamGia.getPhanTramGiam()) ||
        maGiamGia.getPhanTramGiam() <= 0.0 ||
        maGiamGia.getPhanTramGiam() > MAX_DISCOUNT_PERCENT ||
        maGiamGia.getSoLuongConLai() < 0 ||
        maGiamGia.getSoLuongConLai() > maGiamGia.getSoLuongToiDa() ||
        this->kiemTraMa(strMaGG))
    {
        return false;
    }
    this->_danhSach.pushBack(maGiamGia);
    return true;
}

void DanhSachMaGiamGia::hienThi() const
{
    if (this->_danhSach.empty())
    {
        cout << "Danh sach ma giam gia rong!" << endl;
        return;
    }

    this->inTieuDeBang();

    const NODEMAGIAMGIA* pCurrent = this->_danhSach.head();
    while (pCurrent != nullptr)
    {
        pCurrent->Data.xuat();
        pCurrent = pCurrent->Next;
    }
}

bool DanhSachMaGiamGia::hienThiTheoMaHang(const string& strMaHang) const
{
    const NODEMAGIAMGIA* pCurrent = this->_danhSach.head();
    bool bCoMa = false;

    while (pCurrent != nullptr)
    {
        if (pCurrent->Data.getMaHang() == strMaHang && pCurrent->Data.getSoLuongConLai() > 0)
        {
            if (!bCoMa)
            {
                this->inTieuDeBang();
                bCoMa = true;
            }
            pCurrent->Data.xuat();
        }
        pCurrent = pCurrent->Next;
    }
    return bCoMa;
}

bool DanhSachMaGiamGia::themMaGiamGia(const DanhSachHangHoa& dsHangHoa)
{
    string strMaGG = "";
    cout << "Nhap ma giam gia moi (VD: KM01, SALE20): ";
    if (!getline(cin, strMaGG))
    {
        return false;
    }
    strMaGG = DanhSachMaGiamGia::trim(strMaGG);

    if (strMaGG.empty() || strMaGG.find_first_of("|\r\n") != string::npos)
    {
        cout << "Ma giam gia khong duoc de trong hoac chua ky tu phan cach!" << endl;
        return false;
    }

    if (this->kiemTraMa(strMaGG))
    {
        cout << "Ma giam gia da ton tai!" << endl;
        return false;
    }

    string strMaHH = "";
    cout << "Nhap ma hang ap dung (4 ky tu): ";
    if (!getline(cin, strMaHH))
    {
        return false;
    }
    strMaHH = DanhSachHangHoa::chuanHoaMaHang(
        DanhSachMaGiamGia::trim(strMaHH));

    if (!DanhSachHangHoa::maHangHopLe(strMaHH) ||
        !dsHangHoa.kiemTraMa(strMaHH))
    {
        cout << "Ma hang khong ton tai trong danh sach hang hoa!" << endl;
        return false;
    }

    MaGiamGia maGiamGiaMoi;
    if (!maGiamGiaMoi.nhap(strMaGG, strMaHH))
    {
        return false;
    }
    if (!this->them(maGiamGiaMoi))
    {
        cout << "Thong tin ma giam gia khong hop le hoac bi trung!" << endl;
        return false;
    }

    cout << "Them ma giam gia thanh cong!" << endl;
    return true;
}

bool DanhSachMaGiamGia::capNhatSoLuong()
{
    string strMaGG = "";

    cout << "Nhap ma giam gia can cap nhat: ";
    if (!getline(cin, strMaGG))
    {
        return false;
    }
    strMaGG = DanhSachMaGiamGia::trim(strMaGG);

    PNODEMAGIAMGIA pCurrent = this->_danhSach.head();
    while (pCurrent != nullptr)
    {
        if (pCurrent->Data.getMaGiamGia() == strMaGG)
        {
            cout << "Nhap so luong con lai moi: ";
            string strSlMoi;
            if (!getline(cin, strSlMoi))
            {
                return false;
            }

            istringstream streamSoLuong(strSlMoi);
            long long iSlMoi = -1;
            if (!(streamSoLuong >> iSlMoi))
            {
                cout << "So luong khong hop le!" << endl;
                return false;
            }
            streamSoLuong >> ws;
            if (!streamSoLuong.eof() || iSlMoi < 0 ||
                iSlMoi > pCurrent->Data.getSoLuongToiDa() ||
                iSlMoi > (numeric_limits<int>::max)())
            {
                cout << "So luong phai la so nguyen tu 0 den so luong toi da ("
                     << pCurrent->Data.getSoLuongToiDa() << ")!" << endl;
                return false;
            }

            if (!pCurrent->Data.setSoLuongConLai(static_cast<int>(iSlMoi)))
            {
                cout << "Khong the cap nhat so luong ma giam gia!" << endl;
                return false;
            }
            cout << "Cap nhat so luong ma giam gia thanh cong!" << endl;
            return true;
        }
        pCurrent = pCurrent->Next;
    }

    cout << "Khong tim thay ma giam gia!" << endl;
    return false;
}

bool DanhSachMaGiamGia::xoaMaGiamGia()
{
    string strMaGG = "";
    cout << "Nhap ma giam gia can xoa: ";
    if (!getline(cin, strMaGG))
    {
        return false;
    }
    strMaGG = DanhSachMaGiamGia::trim(strMaGG);

    if (this->_danhSach.empty())
    {
        cout << "Danh sach ma giam gia rong!" << endl;
        return false;
    }

    if (this->_danhSach.removeIf(
            [&strMaGG](const MaGiamGia& maGiamGia)
            {
                return maGiamGia.getMaGiamGia() == strMaGG;
            }))
    {
        cout << "Xoa ma giam gia thanh cong!" << endl;
        return true;
    }

    cout << "Khong tim thay ma giam gia!" << endl;
    return false;
}

void DanhSachMaGiamGia::docFile(const DanhSachHangHoa& dsHangHoa)
{
    ifstream fileIn(FILE_MA_GIAM_GIA);
    if (!fileIn.is_open())
    {
        return;
    }

    string strDong;
    size_t iDong = 0;
    while (getline(fileIn, strDong))
    {
        ++iDong;
        if (DanhSachMaGiamGia::trim(strDong).empty())
        {
            continue;
        }

        MaGiamGia maGiamGia;
        if (!MaGiamGia::docDong(strDong, maGiamGia))
        {
            cerr << "[Canh bao] Bo qua ma giam gia khong hop le tai dong "
                 << iDong << ".\n";
            continue;
        }

        const string strMaHang =
            DanhSachHangHoa::chuanHoaMaHang(maGiamGia.getMaHang());
        if (!dsHangHoa.kiemTraMa(strMaHang))
        {
            cerr << "[Canh bao] Bo qua ma giam gia tai dong " << iDong
                 << " vi ma hang " << strMaHang << " khong ton tai.\n";
            continue;
        }
        if (this->kiemTraMa(maGiamGia.getMaGiamGia()))
        {
            cerr << "[Canh bao] Bo qua ma giam gia bi trung tai dong "
                 << iDong << ".\n";
            continue;
        }

        const MaGiamGia maGiamGiaHopLe(
            maGiamGia.getMaGiamGia(),
            strMaHang,
            maGiamGia.getSoLuongToiDa(),
            maGiamGia.getPhanTramGiam(),
            maGiamGia.getSoLuongConLai());
        if (!this->them(maGiamGiaHopLe))
        {
            cerr << "[Canh bao] Bo qua ma giam gia khong hop le tai dong "
                 << iDong << ".\n";
        }
    }

    if (fileIn.bad())
    {
        cerr << "[Loi] Xay ra loi khi doc tep " << FILE_MA_GIAM_GIA << ".\n";
    }
    fileIn.close();
}

bool DanhSachMaGiamGia::ghiFile() const
{
    const string strTepTam = string(FILE_MA_GIAM_GIA) + ".tmp";
    const string strTepBackupTam = string(FILE_MA_GIAM_GIA) + ".bak.tmp";
    const string strTepBackup = string(FILE_MA_GIAM_GIA) + ".bak";

    ofstream fileOut(strTepTam.c_str(), ios::out | ios::trunc | ios::binary);
    if (!fileOut.is_open())
    {
        cerr << "[Loi] Khong tao duoc tep tam de luu " << FILE_MA_GIAM_GIA << ".\n";
        return false;
    }

    const NODEMAGIAMGIA* pCurrent = this->_danhSach.head();
    while (pCurrent != nullptr)
    {
        pCurrent->Data.ghiFile(fileOut);
        pCurrent = pCurrent->Next;
    }

    fileOut.flush();
    if (!fileOut)
    {
        fileOut.close();
        DeleteFileA(strTepTam.c_str());
        cerr << "[Loi] Ghi tep tam ma giam gia that bai; tep goc duoc giu nguyen.\n";
        return false;
    }
    fileOut.close();
    if (!fileOut)
    {
        DeleteFileA(strTepTam.c_str());
        cerr << "[Loi] Dong tep tam ma giam gia that bai; tep goc duoc giu nguyen.\n";
        return false;
    }

    if (GetFileAttributesA(FILE_MA_GIAM_GIA) != INVALID_FILE_ATTRIBUTES)
    {
        if (!CopyFileA(FILE_MA_GIAM_GIA, strTepBackupTam.c_str(), FALSE) ||
            !MoveFileExA(strTepBackupTam.c_str(), strTepBackup.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        {
            const DWORD dwLoi = GetLastError();
            DeleteFileA(strTepTam.c_str());
            DeleteFileA(strTepBackupTam.c_str());
            cerr << "[Loi] Khong tao duoc backup ma giam gia (ma loi Windows: "
                 << dwLoi << "); tep goc duoc giu nguyen.\n";
            return false;
        }
    }

    if (!MoveFileExA(strTepTam.c_str(), FILE_MA_GIAM_GIA,
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        const DWORD dwLoi = GetLastError();
        DeleteFileA(strTepTam.c_str());
        cerr << "[Loi] Khong the cap nhat tep ma giam gia (ma loi Windows: "
             << dwLoi << ").\n";
        return false;
    }
    return true;
}
