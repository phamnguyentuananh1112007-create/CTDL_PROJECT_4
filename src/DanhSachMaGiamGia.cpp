#include "DanhSachMaGiamGia.h"
#include "NodeMaGiamGia.h"
#include "Console.h"

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
    const size_t tongChieuRong = static_cast<size_t>(
        COL_WIDTH_MA_GG + COL_WIDTH_MA_HH_GG + COL_WIDTH_SL_TOI_DA +
        COL_WIDTH_PHAN_TRAM + COL_WIDTH_SL_CON_LAI
    );

    Console::setColor(Console::Color::Yellow);
    cout << string(tongChieuRong, '=') << endl;
    Console::setColor(Console::Color::Cyan);
    cout << left
         << setw(COL_WIDTH_MA_GG) << "Ma Giam Gia"
         << setw(COL_WIDTH_MA_HH_GG) << "Ma Hang"
         << setw(COL_WIDTH_SL_TOI_DA) << "SL Toi Da/Mat Hang"
         << setw(COL_WIDTH_PHAN_TRAM) << "Giam (%)"
         << setw(COL_WIDTH_SL_CON_LAI) << "SL Con Lai" << endl;
    Console::setColor(Console::Color::Yellow);
    cout << string(tongChieuRong, '-') << endl;
    Console::setColor(Console::Color::Default);
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
        Console::info("Danh sach ma giam gia rong!");
        return;
    }

    this->inTieuDeBang();

    int iTongMa = 0;
    const NODEMAGIAMGIA* pCurrent = this->_danhSach.head();
    while (pCurrent != nullptr)
    {
        pCurrent->Data.xuat();
        ++iTongMa;
        pCurrent = pCurrent->Next;
    }

    const size_t tongChieuRong = static_cast<size_t>(
        COL_WIDTH_MA_GG + COL_WIDTH_MA_HH_GG + COL_WIDTH_SL_TOI_DA +
        COL_WIDTH_PHAN_TRAM + COL_WIDTH_SL_CON_LAI
    );
    Console::setColor(Console::Color::Yellow);
    cout << string(tongChieuRong, '=') << endl;
    Console::setColor(Console::Color::Cyan);
    cout << "Tong cong: " << iTongMa << " ma giam gia." << endl;
    Console::setColor(Console::Color::Default);
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

    if (bCoMa)
    {
        const size_t tongChieuRong = static_cast<size_t>(
            COL_WIDTH_MA_GG + COL_WIDTH_MA_HH_GG + COL_WIDTH_SL_TOI_DA +
            COL_WIDTH_PHAN_TRAM + COL_WIDTH_SL_CON_LAI
        );
        Console::setColor(Console::Color::Yellow);
        cout << string(tongChieuRong, '=') << endl;
        Console::setColor(Console::Color::Default);
    }
    return bCoMa;
}

bool DanhSachMaGiamGia::themMaGiamGia(const DanhSachHangHoa& dsHangHoa)
{
    string strMaGG = "";
    cout << "Nhap ma giam gia moi (VD: KM01, SALE20 - nhap 0 de huy): ";
    if (!getline(cin, strMaGG))
    {
        return false;
    }
    strMaGG = DanhSachMaGiamGia::trim(strMaGG);

    if (strMaGG == "0")
    {
        Console::warning("Da huy them ma giam gia.");
        return false;
    }

    if (strMaGG.empty() || strMaGG.find_first_of("|\r\n") != string::npos)
    {
        Console::error("Ma giam gia khong duoc de trong hoac chua ky tu phan cach!");
        return false;
    }

    if (this->kiemTraMa(strMaGG))
    {
        Console::error("Ma giam gia da ton tai!");
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
        Console::error("Ma hang khong ton tai trong danh sach hang hoa!");
        return false;
    }

    MaGiamGia maGiamGiaMoi;
    if (!maGiamGiaMoi.nhap(strMaGG, strMaHH))
    {
        return false;
    }
    if (!this->them(maGiamGiaMoi))
    {
        Console::error("Thong tin ma giam gia khong hop le hoac bi trung!");
        return false;
    }

    cout << endl;
    Console::success("Them ma giam gia thanh cong!");
    return true;
}

bool DanhSachMaGiamGia::capNhatSoLuong()
{
    string strMaGG = "";

    cout << "Nhap ma giam gia can cap nhat (nhap 0 de huy): ";
    if (!getline(cin, strMaGG))
    {
        return false;
    }
    strMaGG = DanhSachMaGiamGia::trim(strMaGG);

    if (strMaGG == "0")
    {
        Console::warning("Da huy cap nhat ma giam gia.");
        return false;
    }

    PNODEMAGIAMGIA pCurrent = this->_danhSach.head();
    while (pCurrent != nullptr)
    {
        if (pCurrent->Data.getMaGiamGia() == strMaGG)
        {
            cout << "Nhap so luong con lai moi (toi da " << pCurrent->Data.getSoLuongToiDa() << "): ";
            string strSlMoi;
            if (!getline(cin, strSlMoi))
            {
                return false;
            }

            istringstream streamSoLuong(strSlMoi);
            long long iSlMoi = -1;
            if (!(streamSoLuong >> iSlMoi))
            {
                Console::error("So luong khong hop le!");
                return false;
            }
            streamSoLuong >> ws;
            if (!streamSoLuong.eof() || iSlMoi < 0 ||
                iSlMoi > pCurrent->Data.getSoLuongToiDa() ||
                iSlMoi > (numeric_limits<int>::max)())
            {
                Console::error("So luong phai la so nguyen tu 0 den so luong toi da (" +
                               to_string(pCurrent->Data.getSoLuongToiDa()) + ")!");
                return false;
            }

            if (!pCurrent->Data.setSoLuongConLai(static_cast<int>(iSlMoi)))
            {
                Console::error("Khong the cap nhat so luong ma giam gia!");
                return false;
            }
            cout << endl;
            Console::success("Cap nhat so luong ma giam gia thanh cong!");
            return true;
        }
        pCurrent = pCurrent->Next;
    }

    Console::error("Khong tim thay ma giam gia!");
    return false;
}

bool DanhSachMaGiamGia::xoaMaGiamGia()
{
    string strMaGG = "";
    cout << "Nhap ma giam gia can xoa (nhap 0 de huy): ";
    if (!getline(cin, strMaGG))
    {
        return false;
    }
    strMaGG = DanhSachMaGiamGia::trim(strMaGG);

    if (strMaGG == "0")
    {
        Console::warning("Da huy xoa ma giam gia.");
        return false;
    }

    if (this->_danhSach.empty())
    {
        Console::info("Danh sach ma giam gia rong!");
        return false;
    }

    if (!this->kiemTraMa(strMaGG))
    {
        Console::error("Khong tim thay ma giam gia!");
        return false;
    }

    bool bDaHuy = false;
    if (!Console::yesNo("Xac nhan xoa ma giam gia " + strMaGG + "?", bDaHuy) || bDaHuy)
    {
        Console::warning("Da huy thao tac xoa ma giam gia.");
        return false;
    }

    if (this->_danhSach.removeIf(
            [&strMaGG](const MaGiamGia& maGiamGia)
            {
                return maGiamGia.getMaGiamGia() == strMaGG;
            }))
    {
        Console::success("Xoa ma giam gia thanh cong!");
        return true;
    }

    Console::error("Khong tim thay ma giam gia!");
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
            cerr << "[!] Bo qua ma giam gia khong hop le tai dong "
                 << iDong << ".\n";
            continue;
        }

        const string strMaHang =
            DanhSachHangHoa::chuanHoaMaHang(maGiamGia.getMaHang());
        if (!dsHangHoa.kiemTraMa(strMaHang))
        {
            cerr << "[!] Bo qua ma giam gia tai dong " << iDong
                 << " vi ma hang " << strMaHang << " khong ton tai.\n";
            continue;
        }
        if (this->kiemTraMa(maGiamGia.getMaGiamGia()))
        {
            cerr << "[!] Bo qua ma giam gia bi trung tai dong "
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
            cerr << "[!] Bo qua ma giam gia khong hop le tai dong "
                 << iDong << ".\n";
        }
    }

    if (fileIn.bad())
    {
        cerr << "[-] Xay ra loi khi doc tep " << FILE_MA_GIAM_GIA << ".\n";
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
        cerr << "[-] Khong tao duoc tep tam de luu " << FILE_MA_GIAM_GIA << ".\n";
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
        cerr << "[-] Ghi tep tam ma giam gia that bai; tep goc duoc giu nguyen.\n";
        return false;
    }
    fileOut.close();
    if (!fileOut)
    {
        DeleteFileA(strTepTam.c_str());
        cerr << "[-] Dong tep tam ma giam gia that bai; tep goc duoc giu nguyen.\n";
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
            cerr << "[-] Khong tao duoc backup ma giam gia (ma loi Windows: "
                 << dwLoi << "); tep goc duoc giu nguyen.\n";
            return false;
        }
    }

    if (!MoveFileExA(strTepTam.c_str(), FILE_MA_GIAM_GIA,
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        const DWORD dwLoi = GetLastError();
        DeleteFileA(strTepTam.c_str());
        cerr << "[-] Khong the cap nhat tep ma giam gia (ma loi Windows: "
             << dwLoi << ").\n";
        return false;
    }
    return true;
}