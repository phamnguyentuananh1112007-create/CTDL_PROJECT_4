#include "DanhSachHangHoa.h"

std::string DanhSachHangHoa::trim(const std::string& strInput)
{
    const std::string::size_type iDau = strInput.find_first_not_of(" \t\r\n");
    if (iDau == std::string::npos)
    {
        return std::string();
    }

    const std::string::size_type iCuoi = strInput.find_last_not_of(" \t\r\n");
    return strInput.substr(iDau, iCuoi - iDau + 1);
}

DanhSachHangHoa::DanhSachHangHoa() noexcept
    : _hangHoa(), _setMaHang()
{
}

DanhSachHangHoa::DanhSachHangHoa(const DanhSachHangHoa& danhSachKhac)
    : _hangHoa(danhSachKhac._hangHoa),
      _setMaHang(danhSachKhac._setMaHang)
{
}

DanhSachHangHoa& DanhSachHangHoa::operator=(const DanhSachHangHoa& danhSachKhac)
{
    if (this != &danhSachKhac)
    {
        DanhSachHangHoa danhSachTam(danhSachKhac);
        this->swap(danhSachTam);
    }

    return *this;
}

DanhSachHangHoa::DanhSachHangHoa(DanhSachHangHoa&& danhSachKhac) noexcept
    : _hangHoa(std::move(danhSachKhac._hangHoa)),
      _setMaHang(std::move(danhSachKhac._setMaHang))
{
    danhSachKhac._setMaHang.clear();
}

DanhSachHangHoa& DanhSachHangHoa::operator=(DanhSachHangHoa&& danhSachKhac) noexcept
{
    if (this != &danhSachKhac)
    {
        this->_hangHoa = std::move(danhSachKhac._hangHoa);
        this->_setMaHang = std::move(danhSachKhac._setMaHang);

        danhSachKhac._setMaHang.clear();
    }

    return *this;
}

DanhSachHangHoa::~DanhSachHangHoa()
{
}

void DanhSachHangHoa::swap(DanhSachHangHoa& danhSachKhac) noexcept
{
    this->_hangHoa.swap(danhSachKhac._hangHoa);
    this->_setMaHang.swap(danhSachKhac._setMaHang);
}

std::string DanhSachHangHoa::chuanHoaMaHang(const std::string& strMa)
{
    std::string strKetQua = DanhSachHangHoa::trim(strMa);

    for (std::string::size_type i = 0; i < strKetQua.length(); ++i)
    {
        const unsigned char kyTu = static_cast<unsigned char>(strKetQua[i]);
        strKetQua[i] = static_cast<char>(std::toupper(kyTu));
    }

    return strKetQua;
}

std::string::size_type DanhSachHangHoa::doDaiMaHangBatBuoc() noexcept
{
    return static_cast<std::string::size_type>(MA_HANG_REQUIRED_LENGTH);
}

bool DanhSachHangHoa::maHangHopLe(const std::string& strMa)
{
    return DanhSachHangHoa::chuanHoaMaHang(strMa).length() ==
           DanhSachHangHoa::doDaiMaHangBatBuoc();
}

bool DanhSachHangHoa::empty() const noexcept
{
    return this->_hangHoa.empty();
}

const NODE* DanhSachHangHoa::getHead() const noexcept
{
    return this->_hangHoa.head();
}

bool DanhSachHangHoa::kiemTraMa(const std::string& strMa) const
{
    const std::string strMaChuanHoa = DanhSachHangHoa::chuanHoaMaHang(strMa);
    return this->_setMaHang.find(strMaChuanHoa) != this->_setMaHang.end();
}

bool DanhSachHangHoa::layHangHoaTheoMa(const std::string& strMa, HangHoa& ketQua) const
{
    const std::string strMaChuanHoa = DanhSachHangHoa::chuanHoaMaHang(strMa);
    const NODE* pCurrent = this->_hangHoa.head();

    while (pCurrent != NULL)
    {
        if (pCurrent->Data.getMaHang() == strMaChuanHoa)
        {
            ketQua = pCurrent->Data;
            return true;
        }

        pCurrent = pCurrent->Next;
    }

    return false;
}

bool DanhSachHangHoa::capNhatSoLuongTheoMa(const std::string& strMa, const int iSoLuongMoi)
{
    if (iSoLuongMoi < 0)
    {
        return false;
    }

    const std::string strMaChuanHoa = DanhSachHangHoa::chuanHoaMaHang(strMa);
    PNODE pCurrent = this->_hangHoa.head();

    while (pCurrent != NULL)
    {
        if (pCurrent->Data.getMaHang() == strMaChuanHoa)
        {
            pCurrent->Data.setSoLuong(iSoLuongMoi);
            return true;
        }

        pCurrent = pCurrent->Next;
    }

    return false;
}

bool DanhSachHangHoa::them(const HangHoa& hangHoa)
{
    const std::string strMaChuanHoa =
        DanhSachHangHoa::chuanHoaMaHang(hangHoa.getMaHang());

    // Khong cho phep them truc tiep HangHoa tao bang constructor neu
    // gia ban <= 0, ngay khong ton tai hoac so luong am.
    if (!DanhSachHangHoa::maHangHopLe(strMaChuanHoa) ||
        !hangHoa.duLieuHopLe())
    {
        return false;
    }

    HangHoa hangHoaMoi = hangHoa;
    hangHoaMoi.setMaHang(strMaChuanHoa);

    const std::pair<std::unordered_set<std::string>::iterator, bool> ketQuaChen =
        this->_setMaHang.insert(strMaChuanHoa);
    if (!ketQuaChen.second)
    {
        return false;
    }

    try
    {
        this->_hangHoa.pushBack(hangHoaMoi);
    }
    catch (...)
    {
        this->_setMaHang.erase(strMaChuanHoa);
        throw;
    }

    return true;
}

bool DanhSachHangHoa::xoaTheoMa(const std::string& strMa)
{
    const std::string strMaChuanHoa = DanhSachHangHoa::chuanHoaMaHang(strMa);

    const bool bDaXoa = this->_hangHoa.removeIf(
        [&strMaChuanHoa](const HangHoa& hangHoa)
        {
            return hangHoa.getMaHang() == strMaChuanHoa;
        }
    );
    if (bDaXoa)
    {
        this->_setMaHang.erase(strMaChuanHoa);
    }
    return bDaXoa;
}
