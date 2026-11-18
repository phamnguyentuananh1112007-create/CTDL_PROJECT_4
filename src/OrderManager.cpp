#include "OrderManager.h"
#include "Console.h"

ChiTietDonHang::ChiTietDonHang()
{
    maHang = "";
    soLuong = 0;
    donGia = 0.0;
}

ChiTietDonHang::ChiTietDonHang(const std::string& strMaHang, const int iSoLuong, const double dDonGia)
{
    maHang = strMaHang;
    soLuong = iSoLuong;
    donGia = dDonGia;
}

DonHang::DonHang()
{
    maDonHang = 0;
    trangThai = TRANG_THAI_CHO_XU_LY;
    tenKhachHang = "";
    taiKhoan = "";
    diaChi = "";
    soDienThoai = "";
    ngayDat = "";
    tongTien = 0.0;
    tonKhoDaTru = false;
}

std::string OrderManager::trim(const std::string& strInput)
{
    const std::size_t iDau = strInput.find_first_not_of(" \t\r\n");
    if (iDau == std::string::npos)
    {
        return "";
    }
    const std::size_t iCuoi = strInput.find_last_not_of(" \t\r\n");
    return strInput.substr(iDau, iCuoi - iDau + 1);
}

bool OrderManager::coKyTuPhanCach(const std::string& strInput)
{
    return strInput.find('|') != std::string::npos ||
           strInput.find('\n') != std::string::npos ||
           strInput.find('\r') != std::string::npos;
}

bool OrderManager::ngayHopLe(const std::string& strNgay)
{
    // Chap nhan ca dd/MM/yyyy (10 ky tu, tuong thich don cu)
    // va dd/MM/yyyy HH:mm:ss (19 ky tu, dinh dang moi co gio phut giay).
    if (strNgay.size() != 10 && strNgay.size() != 19)
    {
        return false;
    }
    if (strNgay[2] != '/' || strNgay[5] != '/')
    {
        return false;
    }

    for (std::size_t i = 0; i < 10; ++i)
    {
        if (i != 2 && i != 5 && (strNgay[i] < '0' || strNgay[i] > '9'))
        {
            return false;
        }
    }

    const int iNgay = (strNgay[0] - '0') * 10 + (strNgay[1] - '0');
    const int iThang = (strNgay[3] - '0') * 10 + (strNgay[4] - '0');
    const int iNam = (strNgay[6] - '0') * 1000 + (strNgay[7] - '0') * 100 +
                     (strNgay[8] - '0') * 10 + (strNgay[9] - '0');

    if (iNam < 1 || iThang < 1 || iThang > 12)
    {
        return false;
    }

    static const int SO_NGAY_TRONG_THANG[] =
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int iSoNgayToiDa = SO_NGAY_TRONG_THANG[iThang - 1];
    const bool bNamNhuan = (iNam % 400 == 0) ||
                           (iNam % 4 == 0 && iNam % 100 != 0);
    if (iThang == 2 && bNamNhuan)
    {
        iSoNgayToiDa = 29;
    }
    if (iNgay < 1 || iNgay > iSoNgayToiDa)
    {
        return false;
    }

    // Neu co them phan gio phut giay (dd/MM/yyyy HH:mm:ss)
    if (strNgay.size() == 19)
    {
        if (strNgay[10] != ' ' || strNgay[13] != ':' || strNgay[16] != ':')
        {
            return false;
        }
        for (std::size_t i = 11; i < 19; ++i)
        {
            if (i != 13 && i != 16 && (strNgay[i] < '0' || strNgay[i] > '9'))
            {
                return false;
            }
        }
        const int iGio = (strNgay[11] - '0') * 10 + (strNgay[12] - '0');
        const int iPhut = (strNgay[14] - '0') * 10 + (strNgay[15] - '0');
        const int iGiay = (strNgay[17] - '0') * 10 + (strNgay[18] - '0');

        if (iGio < 0 || iGio > 23 || iPhut < 0 || iPhut > 59 || iGiay < 0 || iGiay > 59)
        {
            return false;
        }
    }

    return true;
}

bool OrderManager::ngayKhongTuongLai(const std::string& strNgay)
{
    if (!OrderManager::ngayHopLe(strNgay))
    {
        return false;
    }

    std::tm thoiGianDon{};
    thoiGianDon.tm_mday = (strNgay[0] - '0') * 10 + (strNgay[1] - '0');
    thoiGianDon.tm_mon = (strNgay[3] - '0') * 10 + (strNgay[4] - '0') - 1;
    thoiGianDon.tm_year =
        (strNgay[6] - '0') * 1000 + (strNgay[7] - '0') * 100 +
        (strNgay[8] - '0') * 10 + (strNgay[9] - '0') - 1900;
    if (strNgay.size() == 19)
    {
        thoiGianDon.tm_hour = (strNgay[11] - '0') * 10 + (strNgay[12] - '0');
        thoiGianDon.tm_min = (strNgay[14] - '0') * 10 + (strNgay[15] - '0');
        thoiGianDon.tm_sec = (strNgay[17] - '0') * 10 + (strNgay[18] - '0');
    }
    thoiGianDon.tm_isdst = -1;

    const std::time_t thoiDiemDon = std::mktime(&thoiGianDon);
    if (thoiDiemDon == static_cast<std::time_t>(-1))
    {
        return false;
    }
    return thoiDiemDon <= std::time(nullptr);
}

bool OrderManager::truongBatBuocHopLe(const std::string& strInput)
{
    return !trim(strInput).empty() && !coKyTuPhanCach(strInput);
}

bool OrderManager::maHangFileHopLe(const std::string& strMaHang)
{
    return DanhSachHangHoa::maHangHopLe(strMaHang) &&
           strMaHang.find(',') == std::string::npos &&
           strMaHang.find(';') == std::string::npos &&
           strMaHang.find('|') == std::string::npos;
}

bool OrderManager::tepTonTai(const std::string& strTenTep)
{
    std::ifstream fileIn(strTenTep.c_str(), std::ios::in | std::ios::binary);
    return fileIn.good();
}

int OrderManager::timViTriDonTheoMa(const int iMaDon) const
{
    for (std::size_t i = 0; i < _dsDonHang.size(); ++i)
    {
        if (_dsDonHang[i].maDonHang == iMaDon)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void OrderManager::taiTaoHangDoiChoXuLy()
{
    Queue<DonHang> qMoi;
    for (std::size_t i = 0; i < _dsDonHang.size(); ++i)
    {
        if (_dsDonHang[i].trangThai == TRANG_THAI_CHO_XU_LY)
        {
            qMoi.push(_dsDonHang[i]);
        }
    }
    _qDonChoXuLy.swap(qMoi);
}

bool OrderManager::chuyenChuoiSangSoNguyen(const std::string& strInput, int& iKetQua)
{
    std::istringstream iss(strInput);
    char cThua = '\0';
    if (!(iss >> iKetQua) || (iss >> cThua))
    {
        return false;
    }
    return true;
}

bool OrderManager::chuyenChuoiSangSoThuc(const std::string& strInput, double& dKetQua)
{
    std::istringstream iss(strInput);
    char cThua = '\0';
    if (!(iss >> dKetQua) || (iss >> cThua))
    {
        return false;
    }
    return std::isfinite(dKetQua) && dKetQua >= 0.0;
}

bool OrderManager::nhapSoNguyen(const std::string& strThongBao, const int iGiaTriNhoNhat, int& iKetQua) const
{
    std::string strNhap = "";
    while (true)
    {
        std::cout << strThongBao;
        if (!std::getline(std::cin, strNhap))
        {
            return false;
        }
        if (chuyenChuoiSangSoNguyen(trim(strNhap), iKetQua) && iKetQua >= iGiaTriNhoNhat)
        {
            return true;
        }
        std::cout << "Gia tri khong hop le. Vui long nhap so nguyen >= "
             << iGiaTriNhoNhat << ".\n";
    }
}

bool OrderManager::nhapChuoiBatBuoc(const std::string& strThongBao, std::string& strKetQua) const
{
    while (true)
    {
        std::cout << strThongBao;
        if (!std::getline(std::cin, strKetQua))
        {
            return false;
        }
        strKetQua = trim(strKetQua);
        if (truongBatBuocHopLe(strKetQua))
        {
            return true;
        }
        if (coKyTuPhanCach(strKetQua))
        {
            std::cout << "Thong tin khong duoc chua ky tu | vi day la ky tu phan cach trong tep DonHang.txt.\n";
        }
        else
        {
            std::cout << "Thong tin khong duoc de trong. Vui long nhap lai.\n";
        }
    }
}

std::string OrderManager::layNgayHienTai()
{
    const std::time_t now = std::time(nullptr);
    std::tm thoiGian{};
#ifdef _WIN32
    if (localtime_s(&thoiGian, &now) != 0)
    {
        return "";
    }
#else
    const std::tm* pThoiGian = std::localtime(&now);
    if (pThoiGian == nullptr)
    {
        return "";
    }
    thoiGian = *pThoiGian;
#endif
    char szNgayGio[32] = {0};
    std::strftime(szNgayGio, sizeof(szNgayGio), "%d/%m/%Y %H:%M:%S", &thoiGian);
    return std::string(szNgayGio);
}

std::string OrderManager::dinhDangThoiGianHienThi(const std::string& strNgayDat)
{
    if (strNgayDat.size() == 19)
    {
        // Tach dd/MM/yyyy (0..9) va HH:mm:ss (11..18)
        const std::string strNgay = strNgayDat.substr(0, 10);
        const std::string strGio = strNgayDat.substr(11, 8);
        return strGio + " - Ngay " + strNgay;
    }
    return "Ngay " + strNgayDat + " (khong co gio cu the)";
}

int OrderManager::timDonChoXuLyDauTien() const
{
    if (_qDonChoXuLy.empty())
    {
        return -1;
    }

    const int iMaDonDau = _qDonChoXuLy.front().maDonHang;
    const int iViTri = timViTriDonTheoMa(iMaDonDau);
    if (iViTri < 0 || _dsDonHang[static_cast<std::size_t>(iViTri)].trangThai != TRANG_THAI_CHO_XU_LY)
    {
        return -1;
    }
    return iViTri;
}

long long OrderManager::tinhSoLuongChoChuaTru(const std::string& strMaHang) const
{
    long long iTong = 0;
    for (std::size_t i = 0; i < _dsDonHang.size(); ++i)
    {
        const DonHang& donHang = _dsDonHang[i];
        if (donHang.trangThai != TRANG_THAI_CHO_XU_LY || donHang.tonKhoDaTru)
        {
            continue;
        }
        for (std::size_t j = 0; j < donHang.chiTiet.size(); ++j)
        {
            if (donHang.chiTiet[j].maHang == strMaHang)
            {
                iTong += donHang.chiTiet[j].soLuong;
            }
        }
    }
    return iTong;
}

bool OrderManager::hopNhatChiTiet(std::vector<ChiTietDonHang>& dsChiTiet,
                           const ChiTietDonHang& chiTietMoi)
{
    for (std::size_t i = 0; i < dsChiTiet.size(); ++i)
    {
        if (dsChiTiet[i].maHang == chiTietMoi.maHang)
        {
            if (dsChiTiet[i].soLuong >
                (std::numeric_limits<int>::max)() - chiTietMoi.soLuong)
            {
                return false;
            }
            dsChiTiet[i].soLuong += chiTietMoi.soLuong;
            return true;
        }
    }
    dsChiTiet.push_back(chiTietMoi);
    return true;
}

bool OrderManager::docChiTietDonHang(const std::string& strDuLieu, std::vector<ChiTietDonHang>& dsChiTiet) const
{
    dsChiTiet.clear();
    if (trim(strDuLieu).empty())
    {
        return false;
    }

    std::istringstream issDanhSach(strDuLieu);
    std::string strMotChiTiet = "";
    while (std::getline(issDanhSach, strMotChiTiet, ';'))
    {
        std::istringstream issChiTiet(strMotChiTiet);
        std::string strMaHang = "";
        std::string strSoLuong = "";
        std::string strDonGia = "";

        if (!std::getline(issChiTiet, strMaHang, ',') ||
            !std::getline(issChiTiet, strSoLuong, ',') ||
            !std::getline(issChiTiet, strDonGia))
        {
            return false;
        }

        int iSoLuong = 0;
        double dDonGia = 0.0;
        strMaHang = DanhSachHangHoa::chuanHoaMaHang(trim(strMaHang));
        if (!maHangFileHopLe(strMaHang) ||
            !chuyenChuoiSangSoNguyen(trim(strSoLuong), iSoLuong) || iSoLuong <= 0 ||
            !chuyenChuoiSangSoThuc(trim(strDonGia), dDonGia) || dDonGia < 0.0)
        {
            return false;
        }

        if (!hopNhatChiTiet(dsChiTiet, ChiTietDonHang(strMaHang, iSoLuong, dDonGia)))
        {
            return false;
        }
    }
    return !dsChiTiet.empty();
}

bool OrderManager::docMotDonHang(const std::string& strDong, DonHang& donHang) const
{
    std::istringstream iss(strDong);
    std::vector<std::string> truong;
    std::string strTruong = "";
    while (std::getline(iss, strTruong, '|'))
    {
        truong.push_back(strTruong);
    }

    if (truong.size() != 8 && truong.size() != 9 && truong.size() != 10)
    {
        return false;
    }

    std::string strMaDon = trim(truong[0]);
    donHang.trangThai = trim(truong[1]);
    const std::size_t iOffset = (truong.size() >= 9) ? 1 : 0;
    if (iOffset == 1)
    {
        donHang.tenKhachHang = trim(truong[2]);
        donHang.taiKhoan = trim(truong[3]);
    }
    else
    {
        donHang.taiKhoan = trim(truong[2]);
        donHang.tenKhachHang = donHang.taiKhoan;
    }
    donHang.diaChi = trim(truong[3 + iOffset]);
    donHang.soDienThoai = trim(truong[4 + iOffset]);
    donHang.ngayDat = trim(truong[5 + iOffset]);
    const std::string strTongTien = trim(truong[6 + iOffset]);
    const std::string strChiTiet = trim(truong[7 + iOffset]);

    // Dinh dang 10 truong moi co them co tonKhoDaTru (1/0).
    // Cac don cu: DA_XU_LY da tru kho tai thoi diem xu ly; CHO_XU_LY chua tru;
    // DA_HUY khong con giu hang. Don CHO_XU_LY cu se duoc giu kho mot lan khi docFile.
    if (truong.size() == 10)
    {
        const std::string strTonKhoDaTru = trim(truong[9]);
        if (strTonKhoDaTru != "0" && strTonKhoDaTru != "1")
        {
            return false;
        }
        donHang.tonKhoDaTru = (strTonKhoDaTru == "1");
    }
    else
    {
        donHang.tonKhoDaTru = (donHang.trangThai == TRANG_THAI_DA_XU_LY);
    }

    if (!chuyenChuoiSangSoNguyen(strMaDon, donHang.maDonHang) ||
        donHang.maDonHang <= 0 ||
        !truongBatBuocHopLe(donHang.tenKhachHang) ||
        !truongBatBuocHopLe(donHang.tenKhachHang) ||
        !truongBatBuocHopLe(donHang.taiKhoan) ||
        !truongBatBuocHopLe(donHang.diaChi) ||
        !truongBatBuocHopLe(donHang.soDienThoai) ||
        !ngayKhongTuongLai(donHang.ngayDat) ||
        !chuyenChuoiSangSoThuc(strTongTien, donHang.tongTien) ||
        (donHang.trangThai != TRANG_THAI_CHO_XU_LY &&
         donHang.trangThai != TRANG_THAI_DA_XU_LY &&
         donHang.trangThai != TRANG_THAI_DA_HUY) ||
        !docChiTietDonHang(strChiTiet, donHang.chiTiet))
    {
        return false;
    }
    if ((donHang.trangThai == TRANG_THAI_DA_XU_LY && !donHang.tonKhoDaTru) ||
        (donHang.trangThai == TRANG_THAI_DA_HUY && donHang.tonKhoDaTru))
    {
        return false;
    }

    for (std::size_t i = 0; i < donHang.chiTiet.size(); ++i)
    {
        HangHoa hangHoa;
        if (!_dsHangHoa.layHangHoaTheoMa(donHang.chiTiet[i].maHang, hangHoa))
        {
            return false;
        }
    }
    return true;
}

void OrderManager::inThongTinDonHang(const DonHang& donHang) const
{
    std::cout << "\nMa don hang: " << donHang.maDonHang << '\n'
         << "Trang thai: " << donHang.trangThai << '\n'
         << "Ten khach hang: " << donHang.tenKhachHang << '\n'
         << "Tai khoan: " << donHang.taiKhoan << '\n'
         << "Dia chi: " << donHang.diaChi << '\n'
         << "So dien thoai: " << donHang.soDienThoai << '\n'
         << "Thoi gian dat don: " << dinhDangThoiGianHienThi(donHang.ngayDat) << '\n'
         << "Chi tiet don hang:\n";

    std::cout << std::left << std::setw(12) << "Ma hang"
         << std::setw(12) << "So luong"
         << std::setw(18) << "Don gia"
         << std::setw(18) << "Thanh tien" << '\n';
    std::cout << "------------------------------------------------------------\n";

    for (std::size_t i = 0; i < donHang.chiTiet.size(); ++i)
    {
        const ChiTietDonHang& chiTiet = donHang.chiTiet[i];
        std::cout << std::left << std::setw(12) << chiTiet.maHang
             << std::setw(12) << chiTiet.soLuong
             << std::setw(18) << std::fixed << std::setprecision(0) << chiTiet.donGia
             << std::setw(18) << std::fixed << std::setprecision(0)
             << chiTiet.donGia * chiTiet.soLuong << '\n';
    }
    std::cout << "Tong tien: " << std::fixed << std::setprecision(0) << donHang.tongTien << " VND\n";
}

bool OrderManager::kiemTraDuHang(const DonHang& donHang, std::string& strLyDo) const
{
    if (!donHang.tonKhoDaTru)
    {
        strLyDo = "Don hang chua giu duoc ton kho. Vui long kiem tra lai so luong ton kho.";
        return false;
    }

    // Ton kho hien tai la so luong con lai sau khi da giu hang cho don nay.
    // Khi xu ly don chi doi trang thai, tuyet doi khong tru kho lan thu hai.
    for (std::size_t i = 0; i < donHang.chiTiet.size(); ++i)
    {
        HangHoa hangHoa;
        if (!_dsHangHoa.layHangHoaTheoMa(donHang.chiTiet[i].maHang, hangHoa))
        {
            strLyDo = "Khong tim thay ma hang " + donHang.chiTiet[i].maHang + " trong kho.";
            return false;
        }
    }
    return true;
}

bool OrderManager::thayDoiTonKhoTheoDon(
    const DonHang& donHang,
    const bool bTruKho,
    std::string& strLyDo)
{
    DanhSachHangHoa danhSachTruoc = _dsHangHoa;

    for (std::size_t i = 0; i < donHang.chiTiet.size(); ++i)
    {
        const ChiTietDonHang& chiTiet = donHang.chiTiet[i];
        HangHoa hangHoa;
        if (!_dsHangHoa.layHangHoaTheoMa(chiTiet.maHang, hangHoa))
        {
            _dsHangHoa.swap(danhSachTruoc);
            strLyDo = "Khong tim thay ma hang " + chiTiet.maHang + " trong kho.";
            return false;
        }
        if (chiTiet.soLuong <= 0)
        {
            _dsHangHoa.swap(danhSachTruoc);
            strLyDo = "So luong trong don hang khong hop le.";
            return false;
        }

        const long long iSoLuongMoi = bTruKho
            ? static_cast<long long>(hangHoa.getSoLuong()) - chiTiet.soLuong
            : static_cast<long long>(hangHoa.getSoLuong()) + chiTiet.soLuong;

        if (iSoLuongMoi < 0 ||
            iSoLuongMoi > static_cast<long long>((std::numeric_limits<int>::max)()))
        {
            _dsHangHoa.swap(danhSachTruoc);
            std::ostringstream oss;
            oss << "Khong the " << (bTruKho ? "giu" : "hoan")
                << " " << chiTiet.soLuong << " san pham " << chiTiet.maHang
                << ". Ton kho hien tai: " << hangHoa.getSoLuong() << ".";
            strLyDo = oss.str();
            return false;
        }

        if (!_dsHangHoa.capNhatSoLuongTheoMa(
                chiTiet.maHang, static_cast<int>(iSoLuongMoi)))
        {
            _dsHangHoa.swap(danhSachTruoc);
            strLyDo = "Cap nhat ton kho that bai cho ma hang " + chiTiet.maHang + ".";
            return false;
        }
    }

    return true;
}

bool OrderManager::thucHienHuyDonTheoMa(const int iMaDon, const std::string& strTaiKhoanKiemTra)
{
    const int iViTri = timViTriDonTheoMa(iMaDon);
    if (iViTri < 0)
    {
        std::cout << "Khong tim thay don hang #" << iMaDon << ".\n";
        return false;
    }

    DonHang& donHang = _dsDonHang[static_cast<std::size_t>(iViTri)];

    if (!strTaiKhoanKiemTra.empty() && donHang.taiKhoan != strTaiKhoanKiemTra)
    {
        std::cout << "Ban khong co quyen huy don hang #" << iMaDon << ".\n";
        return false;
    }

    if (donHang.trangThai != TRANG_THAI_CHO_XU_LY)
    {
        std::cout << "Chi co the huy don hang dang o trang thai " << TRANG_THAI_CHO_XU_LY
                  << ". (Trang thai hien tai: " << donHang.trangThai << ")\n";
        return false;
    }

    inThongTinDonHang(donHang);
    bool bDaHuy = false;
    if (!Console::yesNo("Xac nhan huy don hang #" + std::to_string(iMaDon) + "?", bDaHuy))
    {
        Console::info("Da giu nguyen don hang #" + std::to_string(iMaDon) + ".");
        return false;
    }

    const std::string strTrangThaiCu = donHang.trangThai;
    const bool bTonKhoDaTruCu = donHang.tonKhoDaTru;
    DanhSachHangHoa danhSachKhoTruoc = _dsHangHoa;

    // Don CHO_XU_LY da giu hang khi dat. Huy don phai hoan lai dung so luong.
    if (bTonKhoDaTruCu)
    {
        std::string strLyDoHoanKho = "";
        if (!thayDoiTonKhoTheoDon(donHang, false, strLyDoHoanKho))
        {
            std::cout << "Khong the hoan ton kho khi huy don: " << strLyDoHoanKho << "\n";
            return false;
        }
    }

    donHang.trangThai = TRANG_THAI_DA_HUY;
    donHang.tonKhoDaTru = false;

    std::string strLoiLuuKho = "";
    if (bTonKhoDaTruCu && !_repositoryHangHoa.ghiFile(_dsHangHoa, strLoiLuuKho))
    {
        _dsHangHoa.swap(danhSachKhoTruoc);
        donHang.trangThai = strTrangThaiCu;
        donHang.tonKhoDaTru = bTonKhoDaTruCu;
        std::cout << "Khong the luu ton kho sau khi hoan hang. Huy don that bai. "
                  << strLoiLuuKho << "\n";
        return false;
    }

    if (!ghiFile())
    {
        donHang.trangThai = strTrangThaiCu;
        donHang.tonKhoDaTru = bTonKhoDaTruCu;
        if (bTonKhoDaTruCu)
        {
            _dsHangHoa.swap(danhSachKhoTruoc);
            std::string strLoiKhoiPhuc = "";
            if (!_repositoryHangHoa.ghiFile(_dsHangHoa, strLoiKhoiPhuc))
            {
                std::cerr << "[!] Khong the khoi phuc ton kho sau khi huy don that bai: "
                          << strLoiKhoiPhuc << "\n";
            }
        }
        std::cout << "Khong the luu file DonHang.txt. Huy don hang that bai; ton kho da duoc khoi phuc neu can.\n";
        return false;
    }

    taiTaoHangDoiChoXuLy();
    std::cout << "\n[+] Da huy don hang #" << iMaDon;
    if (bTonKhoDaTruCu)
    {
        std::cout << ". So luong san pham da duoc hoan lai vao ton kho";
    }
    std::cout << ".\n";
    return true;
}

OrderManager::OrderManager(
    DanhSachHangHoa& dsHangHoa,
    DanhSachMaGiamGia& dsMaGiamGia,
    HangHoaRepository& repositoryHangHoa
)
    : _dsHangHoa(dsHangHoa),
      _dsMaGiamGia(dsMaGiamGia),
      _repositoryHangHoa(repositoryHangHoa),
      _iMaDonTiepTheo(1),
      _strMaGiamGiaChoDonTiepTheo()
{
}

void OrderManager::docFile()
{
    _dsDonHang.clear();
    _dsDongDonKhongHopLe.clear();
    _qDonChoXuLy = Queue<DonHang>();
    _iMaDonTiepTheo = 1;

    const std::string strTepChinh = FILE_DON_HANG;
    const std::string strTepSwap = strTepChinh + ".swap";

    if (!tepTonTai(strTepChinh) && tepTonTai(strTepSwap))
    {
        if (std::rename(strTepSwap.c_str(), strTepChinh.c_str()) != 0)
        {
            std::cerr << "[-] Khong the khoi phuc DonHang.txt tu tep .swap.\n";
            return;
        }
    }
    else if (tepTonTai(strTepChinh) && tepTonTai(strTepSwap))
    {
        std::remove(strTepSwap.c_str());
    }

    std::ifstream fileIn(FILE_DON_HANG, std::ios::in | std::ios::binary);
    if (!fileIn.is_open())
    {
        std::ofstream fileMoi(FILE_DON_HANG, std::ios::out | std::ios::binary);
        if (!fileMoi)
        {
            std::cerr << "[-] Khong the tao tep " << FILE_DON_HANG << ".\n";
        }
        return;
    }

    std::unordered_set<int> setMaDonDaDoc;
    std::string strDong = "";
    int iDong = 0;
    while (std::getline(fileIn, strDong))
    {
        ++iDong;
        const std::string strDongGoc = strDong;
        strDong = trim(strDong);
        if (strDong.empty())
        {
            continue;
        }

        DonHang donHang;
        if (!docMotDonHang(strDong, donHang))
        {
            std::cerr << "[!] Bo qua dong " << iDong
                 << " khong hop le trong DonHang.txt (kiem tra ngay, truong du lieu va so lieu).\n";
            _dsDongDonKhongHopLe.push_back(strDongGoc);
            continue;
        }

        if (setMaDonDaDoc.find(donHang.maDonHang) != setMaDonDaDoc.end())
        {
            std::cerr << "[!] Bo qua ma don hang bi trung tai dong " << iDong << ".\n";
            _dsDongDonKhongHopLe.push_back(strDongGoc);
            continue;
        }
        setMaDonDaDoc.insert(donHang.maDonHang);

        _iMaDonTiepTheo = std::max(_iMaDonTiepTheo, donHang.maDonHang + 1);
        _dsDonHang.push_back(donHang);
        if (donHang.trangThai == TRANG_THAI_CHO_XU_LY)
        {
            _qDonChoXuLy.push(donHang);
        }
    }

    if (fileIn.bad())
    {
        std::cerr << "[-] Xay ra loi khi doc DonHang.txt.\n";
    }
    fileIn.close();

    // Chuyen cac don CHO_XU_LY cu sang co che giu kho ngay khi dat.
    // Co tonKhoDaTru duoc luu trong DonHang.txt, nen moi don chi duoc tru mot lan.
    DanhSachHangHoa danhSachKhoBanDau = _dsHangHoa;
    std::vector<std::size_t> dsDonDaGiuKho;
    bool bDaThayDoiTonKho = false;
    for (std::size_t i = 0; i < _dsDonHang.size(); ++i)
    {
        DonHang& donHang = _dsDonHang[i];
        if (donHang.trangThai != TRANG_THAI_CHO_XU_LY || donHang.tonKhoDaTru)
        {
            continue;
        }

        std::string strLyDo = "";
        if (thayDoiTonKhoTheoDon(donHang, true, strLyDo))
        {
            donHang.tonKhoDaTru = true;
            dsDonDaGiuKho.push_back(i);
            bDaThayDoiTonKho = true;
        }
        else
        {
            std::cerr << "[!] Chua the giu ton kho cho don hang #"
                      << donHang.maDonHang << ": " << strLyDo << "\n";
        }
    }

    if (bDaThayDoiTonKho)
    {
        std::string strLoiTonKho = "";
        if (!_repositoryHangHoa.ghiFile(_dsHangHoa, strLoiTonKho))
        {
            _dsHangHoa.swap(danhSachKhoBanDau);
            for (std::size_t i = 0; i < dsDonDaGiuKho.size(); ++i)
            {
                _dsDonHang[dsDonDaGiuKho[i]].tonKhoDaTru = false;
            }
            std::cerr << "[-] Khong the luu ton kho khi chuyen don cu sang co che giu hang: "
                      << strLoiTonKho << "\n";
        }
        else if (!ghiFile())
        {
            _dsHangHoa.swap(danhSachKhoBanDau);
            for (std::size_t i = 0; i < dsDonDaGiuKho.size(); ++i)
            {
                _dsDonHang[dsDonDaGiuKho[i]].tonKhoDaTru = false;
            }
            std::string strLoiKhoiPhuc = "";
            if (!_repositoryHangHoa.ghiFile(_dsHangHoa, strLoiKhoiPhuc))
            {
                std::cerr << "[!] Khong the phuc hoi tep ton kho sau loi chuyen doi don cu: "
                          << strLoiKhoiPhuc << "\n";
            }
            std::cerr << "[-] Khong the luu co trang thai giu kho cho don cu. Da khoi phuc ton kho neu co the.\n";
        }
        else if (!strLoiTonKho.empty())
        {
            std::cerr << "[!] Ton kho da duoc luu, nhung backup can duoc kiem tra: "
                      << strLoiTonKho << "\n";
        }
    }

    taiTaoHangDoiChoXuLy();
}

bool OrderManager::ghiFile() const
{
    const std::string strTepChinh = FILE_DON_HANG;
    const std::string strTepTam = strTepChinh + ".tmp";
    const std::string strTepSwap = strTepChinh + ".swap";

    {
        std::ofstream fileOut(strTepTam.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
        if (!fileOut.is_open())
        {
            std::cerr << "[-] Khong tao duoc tep tam cho DonHang.txt.\n";
            return false;
        }

        fileOut << std::setprecision(std::numeric_limits<double>::max_digits10);
        for (std::size_t i = 0; i < _dsDonHang.size(); ++i)
        {
            const DonHang& donHang = _dsDonHang[i];
            if (donHang.maDonHang <= 0 ||
                (donHang.trangThai != TRANG_THAI_CHO_XU_LY &&
                 donHang.trangThai != TRANG_THAI_DA_XU_LY &&
                 donHang.trangThai != TRANG_THAI_DA_HUY) ||
                !truongBatBuocHopLe(donHang.tenKhachHang) ||
                !truongBatBuocHopLe(donHang.taiKhoan) ||
                !truongBatBuocHopLe(donHang.diaChi) ||
                !truongBatBuocHopLe(donHang.soDienThoai) ||
                !ngayKhongTuongLai(donHang.ngayDat) ||
                !std::isfinite(donHang.tongTien) || donHang.tongTien < 0.0 ||
                donHang.chiTiet.empty() ||
                (donHang.trangThai == TRANG_THAI_DA_XU_LY && !donHang.tonKhoDaTru) ||
                (donHang.trangThai == TRANG_THAI_DA_HUY && donHang.tonKhoDaTru))
            {
                fileOut.close();
                std::remove(strTepTam.c_str());
                std::cerr << "[-] Don hang khong hop le; tu choi ghi de tep du lieu.\n";
                return false;
            }

            fileOut << donHang.maDonHang << '|'
                    << donHang.trangThai << '|'
                    << donHang.tenKhachHang << '|'
                    << donHang.taiKhoan << '|'
                    << donHang.diaChi << '|'
                    << donHang.soDienThoai << '|'
                    << donHang.ngayDat << '|'
                    << donHang.tongTien << '|';

            for (std::size_t j = 0; j < donHang.chiTiet.size(); ++j)
            {
                const ChiTietDonHang& chiTiet = donHang.chiTiet[j];
                if (!maHangFileHopLe(chiTiet.maHang) || chiTiet.soLuong <= 0 ||
                    !std::isfinite(chiTiet.donGia) || chiTiet.donGia < 0.0)
                {
                    fileOut.close();
                    std::remove(strTepTam.c_str());
                    std::cerr << "[-] Chi tiet don hang khong hop le; khong ghi tep.\n";
                    return false;
                }
                if (j > 0)
                {
                    fileOut << ';';
                }
                fileOut << chiTiet.maHang << ',' << chiTiet.soLuong << ',' << chiTiet.donGia;
            }
            fileOut << '|' << (donHang.tonKhoDaTru ? 1 : 0) << '\n';
            if (!fileOut)
            {
                fileOut.close();
                std::remove(strTepTam.c_str());
                std::cerr << "[-] Ghi du lieu vao tep tam that bai.\n";
                return false;
            }
        }

        for (std::size_t i = 0; i < _dsDongDonKhongHopLe.size(); ++i)
        {
            fileOut << _dsDongDonKhongHopLe[i] << '\n';
            if (!fileOut)
            {
                fileOut.close();
                std::remove(strTepTam.c_str());
                std::cerr << "[-] Khong the bao toan dong DonHang loi trong tep tam.\n";
                return false;
            }
        }

        fileOut.flush();
        if (!fileOut)
        {
            fileOut.close();
            std::remove(strTepTam.c_str());
            std::cerr << "[-] Khong the flush tep DonHang.txt tam.\n";
            return false;
        }
        fileOut.close();
        if (!fileOut)
        {
            std::remove(strTepTam.c_str());
            std::cerr << "[-] Khong the dong tep DonHang.txt tam.\n";
            return false;
        }
    }

    const bool bCoTepGoc = tepTonTai(strTepChinh);
    if (bCoTepGoc)
    {
        if (tepTonTai(strTepSwap) && std::remove(strTepSwap.c_str()) != 0)
        {
            std::remove(strTepTam.c_str());
            std::cerr << "[-] Khong the don tep swap cu; tep goc duoc giu nguyen.\n";
            return false;
        }

        if (std::rename(strTepChinh.c_str(), strTepSwap.c_str()) != 0)
        {
            std::remove(strTepTam.c_str());
            std::cerr << "[-] Khong the chuyen tep goc sang .swap.\n";
            return false;
        }
    }

    if (std::rename(strTepTam.c_str(), strTepChinh.c_str()) != 0)
    {
        bool bKhoiPhuc = true;
        if (bCoTepGoc)
        {
            bKhoiPhuc = (std::rename(strTepSwap.c_str(), strTepChinh.c_str()) == 0);
        }
        std::remove(strTepTam.c_str());
        std::cerr << (bKhoiPhuc
            ? "[-] Khong the thay tep moi; tep DonHang.txt goc da duoc khoi phuc.\n"
            : "[!] Khong the thay tep moi; tep goc con trong DonHang.txt.swap.\n");
        return false;
    }

    if (bCoTepGoc)
    {
        std::remove(strTepSwap.c_str());
    }
    return true;
}

void OrderManager::datHang(const std::string& strTaiKhoan)
{
    std::vector<ChiTietDonHang> dsGioHang;
    std::string strMaHang = "";
    std::cout << "\n=== TAO DON HANG (" << layNgayHienTai() << ") ===\n";
    std::cout << "Nhap ma hang can mua (nhap 0 de ket thuc chon hang).\n";

    while (true)
    {
        std::cout << "Ma hang: ";
        if (!std::getline(std::cin, strMaHang))
        {
            return;
        }
        strMaHang = DanhSachHangHoa::chuanHoaMaHang(trim(strMaHang));
        if (strMaHang == "0")
        {
            break;
        }
        if (strMaHang.empty())
        {
            std::cout << "Ma hang khong duoc de trong.\n";
            continue;
        }

        HangHoa hangHoa;
        if (!_dsHangHoa.layHangHoaTheoMa(strMaHang, hangHoa))
        {
            std::cout << "Khong tim thay ma hang " << strMaHang << ".\n";
            continue;
        }

        // Ton kho hien tai da tru cac don cho xu ly, vi moi don duoc giu hang ngay khi dat.
        int iDaChonTrongGio = 0;
        for (std::size_t i = 0; i < dsGioHang.size(); ++i)
        {
            if (dsGioHang[i].maHang == strMaHang)
            {
                iDaChonTrongGio = dsGioHang[i].soLuong;
                break;
            }
        }
        const long long iDonCuChuaGiu = tinhSoLuongChoChuaTru(strMaHang);
        const long long iSoLuongCoTheDat =
            static_cast<long long>(hangHoa.getSoLuong()) - iDonCuChuaGiu - iDaChonTrongGio;

        std::cout << "San pham: " << hangHoa.getTenHang()
             << " | Gia: " << std::fixed << std::setprecision(0) << hangHoa.getGiaBan()
             << " VND | Ton kho con lai: " << hangHoa.getSoLuong()
             << " | Don cu chua giu kho: " << iDonCuChuaGiu
             << " | Da chon trong gio: " << iDaChonTrongGio
             << " | Co the dat them: " << std::max(0LL, iSoLuongCoTheDat) << '\n';

        if (iSoLuongCoTheDat <= 0)
        {
            std::cout << "Mat hang nay da het ton kho kha dung.\n";
            continue;
        }

        int iSoLuong = 0;
        if (!nhapSoNguyen("So luong mua: ", 1, iSoLuong))
        {
            return;
        }

        int iDaChon = -1;
        for (std::size_t i = 0; i < dsGioHang.size(); ++i)
        {
            if (dsGioHang[i].maHang == strMaHang)
            {
                iDaChon = static_cast<int>(i);
                break;
            }
        }

        const int iSoLuongCu = (iDaChon >= 0) ? dsGioHang[iDaChon].soLuong : 0;
        const long long iTongCanDat = static_cast<long long>(iSoLuongCu) + iSoLuong;
        const long long iCoTheDat =
            static_cast<long long>(hangHoa.getSoLuong()) - iDonCuChuaGiu;
        if (iTongCanDat > iCoTheDat)
        {
            std::cout << "So luong vuot qua ton kho kha dung. Ban co the dat them toi da "
                 << std::max(0LL, iCoTheDat - iSoLuongCu) << " san pham ma nay.\n";
            continue;
        }

        if (iDaChon >= 0)
        {
            dsGioHang[iDaChon].soLuong += iSoLuong;
        }
        else
        {
            dsGioHang.push_back(ChiTietDonHang(strMaHang, iSoLuong, hangHoa.getGiaBan()));
        }
        std::cout << "Da them san pham vao gio hang.\n";
    }

    if (dsGioHang.empty())
    {
        std::cout << "Gio hang rong. Khong tao don hang.\n";
        return;
    }

    double dTongTien = 0.0;
    for (std::size_t i = 0; i < dsGioHang.size(); ++i)
    {
        dTongTien += dsGioHang[i].donGia * dsGioHang[i].soLuong;
    }

    bool bApDungMaGiamGia = false;
    std::string strMaGiamGiaDaApDung = "";
    int iSoLuongMaGiamGiaCu = 0;
    double dSoTienGiam = 0.0;

    if (!_strMaGiamGiaChoDonTiepTheo.empty())
    {
        MaGiamGia maGiamGia;
        if (_dsMaGiamGia.layMaGiamGiaTheoMa(_strMaGiamGiaChoDonTiepTheo, maGiamGia) &&
            maGiamGia.getSoLuongConLai() > 0)
        {
            for (std::size_t i = 0; i < dsGioHang.size(); ++i)
            {
                if (DanhSachHangHoa::chuanHoaMaHang(dsGioHang[i].maHang) ==
                    DanhSachHangHoa::chuanHoaMaHang(maGiamGia.getMaHang()))
                {
                    const int iSoLuongGiam = std::min(dsGioHang[i].soLuong, maGiamGia.getSoLuongToiDa());
                    dSoTienGiam = dsGioHang[i].donGia * iSoLuongGiam *
                                  maGiamGia.getPhanTramGiam() / 100.0;
                    dTongTien -= dSoTienGiam;
                    bApDungMaGiamGia = true;
                    strMaGiamGiaDaApDung = maGiamGia.getMaGiamGia();
                    iSoLuongMaGiamGiaCu = maGiamGia.getSoLuongConLai();
                    break;
                }
            }
        }

        if (!bApDungMaGiamGia)
        {
            std::cout << "Ma giam gia " << _strMaGiamGiaChoDonTiepTheo
                 << " khong kha dung hoac khong ap dung cho san pham trong gio hang.\n";
            std::cout << "1. Tiep tuc dat hang khong ap ma\n"
                 << "0. Huy tao don hang\n"
                 << "Lua chon: ";
            std::string strLuaChon = "";
            if (!std::getline(std::cin, strLuaChon) || trim(strLuaChon) != "1")
            {
                std::cout << "Da huy tao don. Ma giam gia van duoc giu de thu lai.\n";
                return;
            }
        }
    }

    std::cout << "\n=== XAC NHAN DON HANG ===\n";
    std::cout << std::left << std::setw(12) << "Ma hang" << std::setw(12) << "So luong"
         << std::setw(18) << "Don gia" << std::setw(18) << "Thanh tien" << '\n';
    std::cout << "------------------------------------------------------------\n";
    for (std::size_t i = 0; i < dsGioHang.size(); ++i)
    {
        std::cout << std::left << std::setw(12) << dsGioHang[i].maHang
             << std::setw(12) << dsGioHang[i].soLuong
             << std::setw(18) << std::fixed << std::setprecision(0) << dsGioHang[i].donGia
             << std::setw(18) << std::fixed << std::setprecision(0)
             << dsGioHang[i].donGia * dsGioHang[i].soLuong << '\n';
    }
    if (bApDungMaGiamGia)
    {
        std::cout << "Ma giam gia: " << strMaGiamGiaDaApDung << '\n'
             << "Tien duoc giam: " << std::fixed << std::setprecision(0) << dSoTienGiam << " VND\n";
    }
    std::cout << "Tong thanh toan: " << std::fixed << std::setprecision(0) << dTongTien << " VND\n";

    std::string strTenKhachHang = "";
    std::string strDiaChi = "";
    std::string strSoDienThoai = "";
    if (!nhapChuoiBatBuoc("Nhap ten khach hang: ", strTenKhachHang) ||
        !nhapChuoiBatBuoc("Nhap dia chi giao hang: ", strDiaChi) ||
        !nhapChuoiBatBuoc("Nhap so dien thoai: ", strSoDienThoai))
    {
        return;
    }

    // Lay moc thoi gian ngay tai thoi diem xac nhan dat don
    const std::string strThoiGianDat = layNgayHienTai();
    std::cout << "Thoi gian dat don: " << dinhDangThoiGianHienThi(strThoiGianDat) << '\n';
    bool bDaHuy = false;
    if (!Console::yesNo("Xac nhan tao don hang?", bDaHuy))
    {
        Console::warning("Da huy tao don hang.");
        return;
    }

    if (!truongBatBuocHopLe(strTaiKhoan) || !ngayHopLe(strThoiGianDat))
    {
        std::cout << "Khong the tao don hang do thong tin tai khoan hoac thoi gian dat khong hop le.\n";
        return;
    }

    DonHang donHangMoi;
    donHangMoi.maDonHang = _iMaDonTiepTheo;
    donHangMoi.trangThai = TRANG_THAI_CHO_XU_LY;
    donHangMoi.tenKhachHang = strTenKhachHang;
    donHangMoi.taiKhoan = strTaiKhoan;
    donHangMoi.diaChi = strDiaChi;
    donHangMoi.soDienThoai = strSoDienThoai;
    donHangMoi.ngayDat = strThoiGianDat;
    donHangMoi.tongTien = dTongTien;
    donHangMoi.chiTiet = dsGioHang;
    donHangMoi.tonKhoDaTru = true;

    DanhSachHangHoa danhSachKhoTruoc = _dsHangHoa;
    std::string strLyDoTruKho = "";
    if (!thayDoiTonKhoTheoDon(donHangMoi, true, strLyDoTruKho))
    {
        std::cout << "Khong the giu ton kho cho don hang: " << strLyDoTruKho << "\n";
        return;
    }

    _dsDonHang.push_back(donHangMoi);
    if (bApDungMaGiamGia)
    {
        if (!_dsMaGiamGia.capNhatSoLuongConLaiTheoMa(
                strMaGiamGiaDaApDung, iSoLuongMaGiamGiaCu - 1))
        {
            _dsDonHang.pop_back();
            _dsHangHoa.swap(danhSachKhoTruoc);
            std::cout << "Khong the cap nhat so luong ma giam gia; don hang chua duoc tao.\n";
            return;
        }
        if (!_dsMaGiamGia.ghiFile())
        {
            _dsMaGiamGia.capNhatSoLuongConLaiTheoMa(
                strMaGiamGiaDaApDung, iSoLuongMaGiamGiaCu);
            _dsDonHang.pop_back();
            _dsHangHoa.swap(danhSachKhoTruoc);
            std::cout << "Khong the luu so luong ma giam gia; don hang chua duoc tao.\n";
            return;
        }
    }

    std::string strLoiLuuTonKho = "";
    if (!_repositoryHangHoa.ghiFile(_dsHangHoa, strLoiLuuTonKho))
    {
        _dsDonHang.pop_back();
        _dsHangHoa.swap(danhSachKhoTruoc);
        if (bApDungMaGiamGia)
        {
            _dsMaGiamGia.capNhatSoLuongConLaiTheoMa(strMaGiamGiaDaApDung,
                                                     iSoLuongMaGiamGiaCu);
            _dsMaGiamGia.ghiFile();
        }
        std::cout << "Khong the luu ton kho; don hang chua duoc tao. "
                  << strLoiLuuTonKho << "\n";
        return;
    }

    if (!ghiFile())
    {
        _dsDonHang.pop_back();
        _dsHangHoa.swap(danhSachKhoTruoc);
        std::string strLoiKhoiPhucKho = "";
        if (!_repositoryHangHoa.ghiFile(_dsHangHoa, strLoiKhoiPhucKho))
        {
            std::cerr << "[!] Khong the khoi phuc tep ton kho sau khi tao don that bai: "
                      << strLoiKhoiPhucKho << "\n";
        }
        if (bApDungMaGiamGia)
        {
            _dsMaGiamGia.capNhatSoLuongConLaiTheoMa(strMaGiamGiaDaApDung,
                                                     iSoLuongMaGiamGiaCu);
            if (!_dsMaGiamGia.ghiFile())
            {
                std::cerr << "[-] Khong luu duoc so luong ma giam gia sau khi huy tao don.\n";
            }
        }
        std::cout << "Khong the luu don hang. Don hang chua duoc tao; ton kho da duoc khoi phuc neu ghi file thanh cong.\n";
        return;
    }

    _qDonChoXuLy.push(donHangMoi);
    ++_iMaDonTiepTheo;
    _strMaGiamGiaChoDonTiepTheo = "";
    std::cout << "\nDat hang thanh cong! Ma don hang cua ban: "
         << donHangMoi.maDonHang
         << " (luc " << dinhDangThoiGianHienThi(donHangMoi.ngayDat) << ")\n"
         << "Ton kho da tru ngay khi dat hang. Don hang da duoc them vao hang doi cho xu ly.\n";
}

void OrderManager::menuApMaGiamGia()
{
    if (_strMaGiamGiaChoDonTiepTheo.empty())
    {
        std::cout << "Nhap ma giam gia muon ap dung (nhap 0 de huy): ";
    }
    else
    {
        std::cout << "Ma dang chon: " << _strMaGiamGiaChoDonTiepTheo << '\n'
             << "Nhap ma khac de thay doi, 0 de bo ma: ";
    }

    std::string strMaGG = "";
    if (!std::getline(std::cin, strMaGG))
    {
        return;
    }
    strMaGG = trim(strMaGG);
    if (strMaGG == "0")
    {
        _strMaGiamGiaChoDonTiepTheo = "";
        std::cout << "Da bo ma giam gia cho don tiep theo.\n";
        return;
    }
    if (strMaGG.empty())
    {
        std::cout << "Khong thay doi ma giam gia.\n";
        return;
    }

    MaGiamGia maGiamGia;
    if (!_dsMaGiamGia.layMaGiamGiaTheoMa(strMaGG, maGiamGia))
    {
        std::cout << "Ma giam gia khong ton tai.\n";
        return;
    }
    if (maGiamGia.getSoLuongConLai() <= 0)
    {
        std::cout << "Ma giam gia da het luot su dung.\n";
        return;
    }

    _strMaGiamGiaChoDonTiepTheo = strMaGG;
    std::cout << "Da chon ma giam gia " << strMaGG
         << ". Ma se duoc ap dung neu don co san pham "
         << maGiamGia.getMaHang() << ".\n";
}

bool OrderManager::hienThiDonHangCuaTaiKhoan(const std::string& strTaiKhoan, const bool bChiDonChoXuLy) const
{
    bool bCoDon = false;
    for (std::size_t i = 0; i < _dsDonHang.size(); ++i)
    {
        const DonHang& donHang = _dsDonHang[i];
        if (donHang.taiKhoan == strTaiKhoan &&
            (!bChiDonChoXuLy || donHang.trangThai == TRANG_THAI_CHO_XU_LY))
        {
            if (!bCoDon)
            {
                std::cout << (bChiDonChoXuLy
                    ? "\n=== DON HANG DANG CHO XU LY CUA BAN ===\n"
                    : "\n=== LICH SU DON HANG CUA BAN ===\n");
                bCoDon = true;
            }
            inThongTinDonHang(donHang);
            std::cout << "------------------------------------------------------------\n";
        }
    }

    if (!bCoDon)
    {
        std::cout << (bChiDonChoXuLy
            ? "\nBan khong co don hang nao dang cho xu ly de huy.\n"
            : "\nBan chua co don hang nao.\n");
    }
    return bCoDon;
}

void OrderManager::huyDonHangKhachHang(const std::string& strTaiKhoan)
{
    if (!hienThiDonHangCuaTaiKhoan(strTaiKhoan, true))
    {
        return;
    }

    int iMaDon = 0;
    if (!nhapSoNguyen("\nNhap ma don hang can huy (nhap 0 de quay lai): ", 0, iMaDon))
    {
        return;
    }
    if (iMaDon == 0)
    {
        std::cout << "Da quay lai menu.\n";
        return;
    }

    thucHienHuyDonTheoMa(iMaDon, strTaiKhoan);
}

void OrderManager::huyDonHangAdmin()
{
    if (_qDonChoXuLy.empty())
    {
        std::cout << "\nHien khong co don hang nao dang cho xu ly.\n";
        return;
    }

    hienThiHangCho();

    int iMaDon = 0;
    if (!nhapSoNguyen("\nNhap ma don hang can huy (nhap 0 de quay lai): ", 0, iMaDon))
    {
        return;
    }
    if (iMaDon == 0)
    {
        std::cout << "Da quay lai menu.\n";
        return;
    }

    thucHienHuyDonTheoMa(iMaDon, "");
}

void OrderManager::hienThiHangCho() const
{
    if (_qDonChoXuLy.empty())
    {
        std::cout << "\nHien khong co don hang nao dang cho xu ly.\n";
        return;
    }

    std::cout << "\n=== HANG DOI DON HANG (FIFO) ===\n";
    Queue<DonHang> qTam = _qDonChoXuLy;
    while (!qTam.empty())
    {
        inThongTinDonHang(qTam.front());
        std::cout << "------------------------------------------------------------\n";
        qTam.pop();
    }
}

void OrderManager::hienThiTatCaDonHang() const
{
    if (_dsDonHang.empty())
    {
        std::cout << "Danh sach don hang rong.\n";
        return;
    }

    std::cout << "\n=== TOAN BO DON HANG ===\n";
    for (std::size_t i = 0; i < _dsDonHang.size(); ++i)
    {
        inThongTinDonHang(_dsDonHang[i]);
        std::cout << "------------------------------------------------------------\n";
    }
}

void OrderManager::xuLyDonTiepTheo()
{
    const int iViTri = timDonChoXuLyDauTien();
    if (iViTri < 0)
    {
        std::cout << "Khong co don hang dang cho xu ly.\n";
        return;
    }

    DonHang& donHang = _dsDonHang[static_cast<std::size_t>(iViTri)];
    std::cout << "\nDon hang tiep theo trong hang doi FIFO la don #"
         << donHang.maDonHang << ".\n";
    inThongTinDonHang(donHang);

    // Neu day la don cu chua the giu kho luc khoi dong (vi ton kho khi do khong du),
    // thu giu lai truoc khi xu ly. Neu chua du hang thi don van nam dau hang doi.
    if (!donHang.tonKhoDaTru)
    {
        DanhSachHangHoa danhSachKhoTruoc = _dsHangHoa;
        std::string strLyDoGiuKho = "";
        if (!thayDoiTonKhoTheoDon(donHang, true, strLyDoGiuKho))
        {
            std::cout << "\n[-] Khong the xu ly Khong the giu ton kho cho don #"
                      << donHang.maDonHang << ": " << strLyDoGiuKho << "\n";
            return;
        }

        donHang.tonKhoDaTru = true;
        std::string strLoiLuuKho = "";
        if (!_repositoryHangHoa.ghiFile(_dsHangHoa, strLoiLuuKho))
        {
            _dsHangHoa.swap(danhSachKhoTruoc);
            donHang.tonKhoDaTru = false;
            std::cout << "Khong the luu ton kho khi giu hang cho don: " << strLoiLuuKho << "\n";
            return;
        }
        if (!ghiFile())
        {
            _dsHangHoa.swap(danhSachKhoTruoc);
            donHang.tonKhoDaTru = false;
            std::string strLoiKhoiPhuc = "";
            if (!_repositoryHangHoa.ghiFile(_dsHangHoa, strLoiKhoiPhuc))
            {
                std::cerr << "[!] Khong the khoi phuc ton kho sau khi ghi don that bai: "
                          << strLoiKhoiPhuc << "\n";
            }
            std::cout << "Khong the luu trang thai giu kho cho don. Don van cho xu ly.\n";
            return;
        }
    }

    std::string strLyDo = "";
    if (!kiemTraDuHang(donHang, strLyDo))
    {
        std::cout << "\n[-] Khong the xu ly " << strLyDo << '\n'
             << "Don #" << donHang.maDonHang
             << " van duoc giu o dau hang doi. He thong khong xu ly don phia sau\n"
             << "de dam bao dung thu tu dat truoc (FIFO).\n";
        return;
    }

    bool bDaHuy = false;
    if (!Console::yesNo("Xac nhan xu ly don #" + std::to_string(donHang.maDonHang) + "?", bDaHuy))
    {
        std::cout << "Da huy thao tac. Don hang van dang cho xu ly.\n";
        return;
    }

    // Moi don moi/legacy da duoc tru (giu) kho truoc khi xu ly.
    // Buoc xu ly chi doi trang thai, khong tiep tuc tru so luong lan nua.
    donHang.trangThai = TRANG_THAI_DA_XU_LY;
    if (!ghiFile())
    {
        donHang.trangThai = TRANG_THAI_CHO_XU_LY;
        std::cout << "Khong the luu trang thai don hang. Don van o dau hang doi; ton kho duoc giu nguyen.\n";
        return;
    }

    _qDonChoXuLy.pop();
    std::cout << "\n[+] Da xu ly don hang #" << donHang.maDonHang
         << ". Ton kho da duoc tru tu luc dat hang, khong tru them lan nua. Don da duoc lay khoi Queue FIFO.\n";
}

bool OrderManager::coDonChoTheoMaHang(const std::string& strMaHang) const
{
    const std::string strMaChuanHoa = DanhSachHangHoa::chuanHoaMaHang(strMaHang);
    Queue<DonHang> qTam = _qDonChoXuLy;
    while (!qTam.empty())
    {
        const DonHang& donHang = qTam.front();
        for (std::size_t i = 0; i < donHang.chiTiet.size(); ++i)
        {
            if (donHang.chiTiet[i].maHang == strMaChuanHoa)
            {
                return true;
            }
        }
        qTam.pop();
    }
    return false;
}

void OrderManager::menuQuanLyDonHang()
{
    while (true)
    {
        std::cout << "\n==============================================\n"
             << "             QUAN LY DON HANG                 \n"
             << "==============================================\n"
             << "1. Xem danh sach don hang dang cho (FIFO)\n"
             << "2. Xu ly don hang tiep theo\n"
             << "3. Xem tat ca don hang\n"
             << "4. Huy don hang dang cho\n"
             << "0. Quay lai menu chinh\n"
             << "----------------------------------------------\n"
             << "Nhap lua chon: ";

        std::string strLuaChon = "";
        if (!std::getline(std::cin, strLuaChon))
        {
            return;
        }
        strLuaChon = trim(strLuaChon);

        if (strLuaChon == "0")
        {
            return;
        }
        else if (strLuaChon == "1")
        {
            hienThiHangCho();
            Console::pause();
        }
        else if (strLuaChon == "2")
        {
            xuLyDonTiepTheo();
            Console::pause();
        }
        else if (strLuaChon == "3")
        {
            hienThiTatCaDonHang();
            Console::pause();
        }
        else if (strLuaChon == "4")
        {
            huyDonHangAdmin();
            Console::pause();
        }
        else
        {
            std::cout << "Lua chon khong hop le. Vui long chon lai.\n";
        }
    }
}
