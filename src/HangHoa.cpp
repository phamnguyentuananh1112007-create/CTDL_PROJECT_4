#include "HangHoa.h"

bool HangHoa::chuoiGiaHopLe(const std::string& s, double& gia)
{
    std::istringstream ss(s);
    double tam;

    if (!(ss >> tam))
        return false;

    ss >> std::ws;

    if (!ss.eof() || !std::isfinite(tam) || tam <= 0)
        return false;

    gia = tam;
    return true;
}

bool HangHoa::chuoiSoLuongHopLe(const std::string& s, int& sl)
{
    std::istringstream ss(s);
    int tam;

    if (!(ss >> tam))
        return false;

    ss >> std::ws;

    if (!ss.eof() || tam < 0)
        return false;

    sl = tam;
    return true;
}

HangHoa::HangHoa()
    : _strMaHang(),
      _strTenHang(),
      _strNoiSanXuat(),
      _strMauSac(),
      _dGiaBan(0.0),
      _strNgayNhapKho(),
      _iSoLuong(0)
{
}

HangHoa::HangHoa(
    const std::string& ma,
    const std::string& ten,
    const std::string& noiSX,
    const std::string& mau,
    double gia,
    const std::string& ngay,
    int soLuong
)
    : _strMaHang(ma),
      _strTenHang(ten),
      _strNoiSanXuat(noiSX),
      _strMauSac(mau),
      _dGiaBan(gia),
      _strNgayNhapKho(ngay),
      _iSoLuong(soLuong)
{
}

bool HangHoa::ngayHopLe(const std::string& ngay)
{
    if (ngay.length() != 10 ||
        ngay[2] != '/' || ngay[5] != '/')
        return false;

    for (int i = 0; i < 10; ++i)
    {
        if (i == 2 || i == 5)
            continue;

        if (!std::isdigit(
            static_cast<unsigned char>(ngay[i])))
            return false;
    }

    int d = (ngay[0] - '0') * 10 + ngay[1] - '0';
    int m = (ngay[3] - '0') * 10 + ngay[4] - '0';

    int y = (ngay[6] - '0') * 1000
          + (ngay[7] - '0') * 100
          + (ngay[8] - '0') * 10
          + (ngay[9] - '0');

    if (y < 1 || m < 1 || m > 12)
        return false;

    int soNgay[] =
    {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool namNhuan =
        (y % 400 == 0) ||
        (y % 4 == 0 && y % 100 != 0);

    if (namNhuan)
        soNgay[1] = 29;

    return d >= 1 && d <= soNgay[m - 1];
}

bool HangHoa::duLieuHopLe() const
{
    return std::isfinite(_dGiaBan) &&
           _dGiaBan > 0.0 &&
           ngayHopLe(_strNgayNhapKho) &&
           _iSoLuong >= 0;
}

const std::string& HangHoa::getMaHang() const
{
    return _strMaHang;
}

const std::string& HangHoa::getTenHang() const
{
    return _strTenHang;
}

const std::string& HangHoa::getNoiSanXuat() const
{
    return _strNoiSanXuat;
}

const std::string& HangHoa::getMauSac() const
{
    return _strMauSac;
}

double HangHoa::getGiaBan() const
{
    return _dGiaBan;
}

const std::string& HangHoa::getNgayNhapKho() const
{
    return _strNgayNhapKho;
}

int HangHoa::getSoLuong() const
{
    return _iSoLuong;
}

std::string HangHoa::getmaHang() const
{
    return _strMaHang;
}

std::string HangHoa::gettenHang() const
{
    return _strTenHang;
}

double HangHoa::getgiaBan() const
{
    return _dGiaBan;
}

int HangHoa::getsoLuong() const
{
    return _iSoLuong;
}

void HangHoa::setMaHang(const std::string& ma)
{
    _strMaHang = ma;
}

void HangHoa::setSoLuong(int sl)
{
    if (sl >= 0)
        _iSoLuong = sl;
}

void HangHoa::setsoLuong(int sl)
{
    setSoLuong(sl);
}

bool HangHoa::Nhap(const std::string& ma)
{
    _strMaHang = ma;

    std::string dong;

    std::cout << "Ten Hang: ";
    if (!std::getline(std::cin, _strTenHang))
        return false;

    std::cout << "Noi San Xuat: ";
    if (!std::getline(std::cin, _strNoiSanXuat))
        return false;

    std::cout << "Mau Sac: ";
    if (!std::getline(std::cin, _strMauSac))
        return false;

    while (true)
    {
        std::cout << "Gia Ban: ";

        if (!std::getline(std::cin, dong))
            return false;

        if (chuoiGiaHopLe(dong, _dGiaBan))
            break;

        std::cout << "Gia ban phai lon hon 0. Vui long nhap lai!\n";
    }

    while (true)
    {
        std::cout << "Ngay Nhap Kho (dd/MM/yyyy): ";

        if (!std::getline(std::cin, _strNgayNhapKho))
            return false;

        if (ngayHopLe(_strNgayNhapKho))
            break;

        std::cout << "Ngay khong hop le. Vui long nhap lai!\n";
    }

    while (true)
    {
        std::cout << "So Luong: ";

        if (!std::getline(std::cin, dong))
            return false;

        if (chuoiSoLuongHopLe(dong, _iSoLuong))
            break;

        std::cout << "So luong phai la so nguyen >= 0!\n";
    }

    return true;
}

void HangHoa::Xuat() const
{
    std::cout << std::left
              << std::setw(7) << _strMaHang
              << std::setw(32) << _strTenHang
              << std::setw(15) << _strNoiSanXuat
              << std::setw(12) << _strMauSac
              << std::setw(14) << std::fixed
              << std::setprecision(0) << _dGiaBan
              << std::setw(14) << _strNgayNhapKho
              << std::setw(9) << _iSoLuong
              << '\n';
}

void HangHoa::ghiFile(std::ofstream& file) const
{
    file << std::setprecision(15)
         << _strMaHang << "|"
         << _strTenHang << "|"
         << _strNoiSanXuat << "|"
         << _strMauSac << "|"
         << _dGiaBan << "|"
         << _strNgayNhapKho << "|"
         << _iSoLuong << '\n';
}

bool HangHoa::docFile(std::ifstream& file)
{
    std::string gia;
    std::string sl;

    if (!std::getline(file, _strMaHang, '|'))
        return false;

    if (!std::getline(file, _strTenHang, '|') ||
        !std::getline(file, _strNoiSanXuat, '|') ||
        !std::getline(file, _strMauSac, '|') ||
        !std::getline(file, gia, '|') ||
        !std::getline(file, _strNgayNhapKho, '|') ||
        !std::getline(file, sl))
        return false;

    try
    {
        std::size_t viTriGia = 0;
        std::size_t viTriSL = 0;

        double giaTam = std::stod(gia, &viTriGia);
        int slTam = std::stoi(sl, &viTriSL);

        if (viTriGia != gia.length() ||
            viTriSL != sl.length() ||
            !std::isfinite(giaTam) ||
            giaTam <= 0 ||
            slTam < 0 ||
            !ngayHopLe(_strNgayNhapKho))
            return false;

        _dGiaBan = giaTam;
        _iSoLuong = slTam;
    }
    catch (...)
    {
        return false;
    }

    return true;
}
