#include "HangHoaConsole.h"
#include "Console.h"

std::string HangHoaConsole::toLowerString(const std::string& strInput)
{
    std::string strKetQua = strInput;
    std::transform(
        strKetQua.begin(), strKetQua.end(), strKetQua.begin(),
        [](unsigned char kyTu)
        {
            return static_cast<char>(std::tolower(kyTu));
        }
    );
    return strKetQua;
}

bool HangHoaConsole::ngayHopLe(const std::string& strNgay)
{
    return HangHoa::ngayHopLe(strNgay);
}

bool HangHoaConsole::chuyenChuoiSangSoNguyen(const std::string& strInput, int& iKetQua)
{
    std::istringstream streamNhap(HangHoaConsole::trim(strInput));
    long long iGiaTriTam = 0;

    if (!(streamNhap >> iGiaTriTam))
    {
        return false;
    }

    streamNhap >> std::ws;
    if (!streamNhap.eof() || iGiaTriTam < 0 ||
        iGiaTriTam > static_cast<long long>((std::numeric_limits<int>::max)()))
    {
        return false;
    }

    iKetQua = static_cast<int>(iGiaTriTam);
    return true;
}

bool HangHoaConsole::chuyenChuoiSangSoThuc(const std::string& strInput, double& dKetQua)
{
    std::istringstream streamNhap(HangHoaConsole::trim(strInput));
    double dGiaTriTam = 0.0;

    if (!(streamNhap >> dGiaTriTam))
    {
        return false;
    }

    streamNhap >> std::ws;
    if (!streamNhap.eof() || !std::isfinite(dGiaTriTam) || dGiaTriTam <= 0.0)
    {
        return false;
    }

    dKetQua = dGiaTriTam;
    return true;
}

bool HangHoaConsole::docDong(const std::string& strThongBao, std::string& strKetQua) const
{
    _output << strThongBao;
    return static_cast<bool>(std::getline(_input, strKetQua));
}

bool HangHoaConsole::docChuoiBatBuoc(const std::string& strThongBao, std::string& strKetQua) const
{
    while (true)
    {
        if (!this->docDong(strThongBao, strKetQua))
        {
            return false;
        }

        strKetQua = HangHoaConsole::trim(strKetQua);
        if (strKetQua.empty())
        {
            Console::warning("Thong tin khong duoc de trong. Vui long nhap lai.");
            continue;
        }

        if (strKetQua.find('|') != std::string::npos)
        {
            Console::warning("Thong tin khong duoc chua ky tu '|'. Vui long nhap lai.");
            continue;
        }

        return true;
    }
}

bool HangHoaConsole::docGiaBan(double& dGiaBan) const
{
    std::string strNhap;
    while (true)
    {
        if (!this->docDong("Gia ban: ", strNhap))
        {
            return false;
        }

        if (HangHoaConsole::chuyenChuoiSangSoThuc(strNhap, dGiaBan))
        {
            return true;
        }

        Console::warning("Gia ban khong hop le. Gia ban bat buoc phai lon hon 0. Vui long nhap lai.");
    }
}

bool HangHoaConsole::docNgayNhapKho(std::string& strNgay) const
{
    while (true)
    {
        if (!this->docDong("Ngay nhap kho (dd/MM/yyyy): ", strNgay))
        {
            return false;
        }

        strNgay = HangHoaConsole::trim(strNgay);
        if (HangHoaConsole::ngayHopLe(strNgay))
        {
            return true;
        }

        Console::warning("Ngay khong hop le. Vui long nhap theo dd/MM/yyyy va dam bao ngay ton tai tren lich.");
    }
}

bool HangHoaConsole::docSoLuong(int& iSoLuong) const
{
    std::string strNhap;
    while (true)
    {
        if (!this->docDong("So luong: ", strNhap))
        {
            return false;
        }

        if (HangHoaConsole::chuyenChuoiSangSoNguyen(strNhap, iSoLuong))
        {
            return true;
        }

        Console::warning("So luong khong hop le. Vui long nhap so nguyen >= 0.");
    }
}

void HangHoaConsole::inTieuDeBang() const
{
    const std::size_t tongChieuRong = static_cast<std::size_t>(
        COL_WIDTH_MA + COL_WIDTH_TEN + COL_WIDTH_NOI_SX +
        COL_WIDTH_MAU + COL_WIDTH_GIA + COL_WIDTH_NGAY + COL_WIDTH_SL
    );
    const std::string vienDoi(tongChieuRong, '=');
    const std::string vienDon(tongChieuRong, '-');

    Console::setColor(Console::Color::Yellow);
    _output << vienDoi << '\n';
    Console::setColor(Console::Color::Cyan);
    _output << std::left
            << std::setw(COL_WIDTH_MA) << "Ma"
            << std::setw(COL_WIDTH_TEN) << "Ten Hang"
            << std::setw(COL_WIDTH_NOI_SX) << "Noi SX"
            << std::setw(COL_WIDTH_MAU) << "Mau"
            << std::setw(COL_WIDTH_GIA) << "Gia ban"
            << std::setw(COL_WIDTH_NGAY) << "Ngay nhap"
            << std::setw(COL_WIDTH_SL) << "So luong"
            << '\n';
    Console::setColor(Console::Color::Yellow);
    _output << vienDon << '\n';
    Console::setColor(Console::Color::Default);
}

void HangHoaConsole::inMotHangHoa(const HangHoa& hangHoa) const
{
    _output << std::left
            << std::setw(COL_WIDTH_MA) << Console::fit(hangHoa.getMaHang(), COL_WIDTH_MA - 1)
            << std::setw(COL_WIDTH_TEN) << Console::fit(hangHoa.getTenHang(), COL_WIDTH_TEN - 1)
            << std::setw(COL_WIDTH_NOI_SX) << Console::fit(hangHoa.getNoiSanXuat(), COL_WIDTH_NOI_SX - 1)
            << std::setw(COL_WIDTH_MAU) << Console::fit(hangHoa.getMauSac(), COL_WIDTH_MAU - 1)
            << std::setw(COL_WIDTH_GIA) << std::fixed << std::setprecision(0)
            << hangHoa.getGiaBan()
            << std::setw(COL_WIDTH_NGAY) << hangHoa.getNgayNhapKho();

    if (hangHoa.getSoLuong() == 0)
    {
        Console::setColor(Console::Color::Red);
    }
    else
    {
        Console::setColor(Console::Color::Green);
    }
    _output << std::setw(COL_WIDTH_SL) << hangHoa.getSoLuong() << '\n';
    Console::setColor(Console::Color::Default);
}

HangHoaConsole::HangHoaConsole(
    std::istream& input,
    std::ostream& output)
    : _input(input), _output(output)
{
}

void HangHoaConsole::hienThi(const DanhSachHangHoa& danhSach, const bool chiHangDangBan) const
{
    const NODE* pCurrent = danhSach.getHead();
    bool bDaInTieuDe = false;
    int iTongMatHang = 0;

    while (pCurrent != NULL)
    {
        if (!chiHangDangBan || pCurrent->Data.getSoLuong() > 0)
        {
            if (!bDaInTieuDe)
            {
                this->inTieuDeBang();
                bDaInTieuDe = true;
            }

            this->inMotHangHoa(pCurrent->Data);
            ++iTongMatHang;
        }

        pCurrent = pCurrent->Next;
    }

    if (!bDaInTieuDe)
    {
        Console::info(chiHangDangBan
            ? "Hien khong co hang hoa nao dang ban."
            : "Danh sach hang hoa rong!");
        return;
    }

    const std::size_t tongChieuRong = static_cast<std::size_t>(
        COL_WIDTH_MA + COL_WIDTH_TEN + COL_WIDTH_NOI_SX +
        COL_WIDTH_MAU + COL_WIDTH_GIA + COL_WIDTH_NGAY + COL_WIDTH_SL
    );
    Console::setColor(Console::Color::Yellow);
    _output << std::string(tongChieuRong, '=') << '\n';
    Console::setColor(Console::Color::Cyan);
    _output << "Tong cong: " << iTongMatHang << " mat hang.\n";
    Console::setColor(Console::Color::Default);
}

void HangHoaConsole::hienThiSapXep(
    const DanhSachHangHoa& danhSach,
    const bool bTangDan,
    const bool chiHangDangBan) const
{
    std::vector<HangHoa> dsSapXep;
    const NODE* pCurrent = danhSach.getHead();

    while (pCurrent != NULL)
    {
        if (!chiHangDangBan || pCurrent->Data.getSoLuong() > 0)
        {
            dsSapXep.push_back(pCurrent->Data);
        }
        pCurrent = pCurrent->Next;
    }

    if (dsSapXep.empty())
    {
        Console::info(chiHangDangBan
            ? "Hien khong co hang hoa nao dang ban."
            : "Danh sach hang hoa rong!");
        return;
    }

    HangHoaSapXep::bubbleSort(dsSapXep, bTangDan);

    Console::info(bTangDan
        ? "Danh sach hang hoa sap xep theo gia tu THAP den CAO:"
        : "Danh sach hang hoa sap xep theo gia tu CAO den THAP:");
    this->inTieuDeBang();

    for (std::vector<HangHoa>::const_iterator it = dsSapXep.begin();
         it != dsSapXep.end(); ++it)
    {
        this->inMotHangHoa(*it);
    }

    const std::size_t tongChieuRong = static_cast<std::size_t>(
        COL_WIDTH_MA + COL_WIDTH_TEN + COL_WIDTH_NOI_SX +
        COL_WIDTH_MAU + COL_WIDTH_GIA + COL_WIDTH_NGAY + COL_WIDTH_SL
    );
    Console::setColor(Console::Color::Yellow);
    _output << std::string(tongChieuRong, '=') << '\n';
    Console::setColor(Console::Color::Cyan);
    _output << "Tong cong: " << dsSapXep.size() << " mat hang.\n";
    Console::setColor(Console::Color::Default);
}

void HangHoaConsole::timKiem(const DanhSachHangHoa& danhSach) const
{
    std::string strTuKhoa;
    if (!this->docDong("Nhap ten hang can tim: ", strTuKhoa))
    {
        return;
    }

    strTuKhoa = HangHoaConsole::toLowerString(HangHoaConsole::trim(strTuKhoa));
    if (strTuKhoa.empty())
    {
        Console::warning("Tu khoa tim kiem khong duoc de trong.");
        return;
    }

    bool bDaInTieuDe = false;
    int iSoKetQua = 0;
    const NODE* pCurrent = danhSach.getHead();

    while (pCurrent != NULL)
    {
        const HangHoa& hangHoa = pCurrent->Data;
        const std::string strTenHang =
            HangHoaConsole::toLowerString(hangHoa.getTenHang());

        if (hangHoa.getSoLuong() > 0 &&
            strTenHang.find(strTuKhoa) != std::string::npos)
        {
            if (!bDaInTieuDe)
            {
                _output << '\n';
                Console::info("Cac hang hoa dang ban tim thay:");
                this->inTieuDeBang();
                bDaInTieuDe = true;
            }

            this->inMotHangHoa(hangHoa);
            ++iSoKetQua;
        }

        pCurrent = pCurrent->Next;
    }

    if (!bDaInTieuDe)
    {
        _output << '\n';
        Console::warning("Khong tim thay hang hoa dang ban phu hop!");
        return;
    }

    const std::size_t tongChieuRong = static_cast<std::size_t>(
        COL_WIDTH_MA + COL_WIDTH_TEN + COL_WIDTH_NOI_SX +
        COL_WIDTH_MAU + COL_WIDTH_GIA + COL_WIDTH_NGAY + COL_WIDTH_SL
    );
    Console::setColor(Console::Color::Yellow);
    _output << std::string(tongChieuRong, '=') << '\n';
    Console::setColor(Console::Color::Cyan);
    _output << "Tim thay: " << iSoKetQua << " mat hang phu hop.\n";
    Console::setColor(Console::Color::Default);
}

bool HangHoaConsole::themHangHoa(DanhSachHangHoa& danhSach) const
{
    std::string strMa;
    std::ostringstream streamThongBao;
    streamThongBao << "Nhap ma hang ("
                   << DanhSachHangHoa::doDaiMaHangBatBuoc()
                   << " ky tu, nhap 0 de huy): ";
    const std::string strThongBao = streamThongBao.str();

    while (true)
    {
        if (!this->docDong(strThongBao, strMa))
        {
            return false;
        }

        strMa = DanhSachHangHoa::chuanHoaMaHang(strMa);

        if (strMa == "0")
        {
            Console::warning("Da huy them hang hoa.");
            return false;
        }

        if (!DanhSachHangHoa::maHangHopLe(strMa))
        {
            Console::warning("Ma hang phai co dung " +
                             std::to_string(DanhSachHangHoa::doDaiMaHangBatBuoc()) +
                             " ky tu. Vui long nhap lai.");
            continue;
        }

        if (danhSach.kiemTraMa(strMa))
        {
            Console::warning("Ma hang da ton tai. Vui long nhap ma khac.");
            continue;
        }

        break;
    }

    std::string strTenHang;
    std::string strNoiSanXuat;
    std::string strMauSac;
    std::string strNgayNhapKho;
    double dGiaBan = 0.0;
    int iSoLuong = 0;

    if (!this->docChuoiBatBuoc("Ten hang: ", strTenHang) ||
        !this->docChuoiBatBuoc("Noi san xuat: ", strNoiSanXuat) ||
        !this->docChuoiBatBuoc("Mau sac: ", strMauSac) ||
        !this->docGiaBan(dGiaBan) ||
        !this->docNgayNhapKho(strNgayNhapKho) ||
        !this->docSoLuong(iSoLuong))
    {
        Console::warning("Da huy thao tac do ket thuc dau vao.");
        return false;
    }

    const HangHoa hangHoaMoi(
        strMa, strTenHang, strNoiSanXuat, strMauSac,
        dGiaBan, strNgayNhapKho, iSoLuong
    );

    if (!hangHoaMoi.duLieuHopLe())
    {
        Console::error("Khong the them hang hoa: gia ban phai > 0 va ngay nhap kho phai la ngay hop le.");
        return false;
    }

    if (!danhSach.them(hangHoaMoi))
    {
        Console::error("Khong the them hang hoa. Ma hang co the da ton tai.");
        return false;
    }

    _output << '\n';
    Console::success("Them hang hoa thanh cong!");
    return true;
}

bool HangHoaConsole::xoaHangHoa(
    DanhSachHangHoa& danhSach,
    const std::function<bool(const std::string&)>& kiemTraDangCho) const
{
    std::string strMa;

    while (true)
    {
        if (!this->docDong("Nhap ma hang can xoa (nhap 0 de huy): ", strMa))
        {
            return false;
        }

        strMa = DanhSachHangHoa::chuanHoaMaHang(strMa);
        if (strMa == "0")
        {
            Console::warning("Da huy xoa hang hoa.");
            return false;
        }

        if (danhSach.kiemTraMa(strMa))
        {
            if (kiemTraDangCho && kiemTraDangCho(strMa))
            {
                Console::error("Khong the xoa ma hang " + strMa +
                               " vi van con don hang dang cho xu ly tham chieu den mat hang nay.");
                return false;
            }
            break;
        }

        Console::warning("Khong tim thay ma hang. Vui long nhap lai.");
    }

    while (true)
    {
        bool bDaHuy = false;
        const bool bDongY = Console::yesNo("Xac nhan xoa hang hoa nay?", bDaHuy);
        if (bDaHuy)
        {
            Console::warning("Da huy thao tac xoa hang hoa.");
            return false;
        }
        if (bDongY)
        {
            if (danhSach.xoaTheoMa(strMa))
            {
                Console::success("Da xoa hang hoa " + strMa + " thanh cong.");
                return true;
            }

            Console::error("Khong the xoa hang hoa " + strMa + ": du lieu da thay doi hoac ma khong con ton tai.");
            return false;
        }
        Console::warning("Da huy thao tac xoa hang hoa.");
        return false;
    }
}

bool HangHoaConsole::capNhatSoLuong(DanhSachHangHoa& danhSach) const
{
    std::string strMa;

    while (true)
    {
        if (!this->docDong("Nhap ma hang can cap nhat (nhap 0 de huy): ", strMa))
        {
            return false;
        }

        strMa = DanhSachHangHoa::chuanHoaMaHang(strMa);
        if (strMa == "0")
        {
            Console::warning("Da huy cap nhat so luong.");
            return false;
        }

        if (danhSach.kiemTraMa(strMa))
        {
            break;
        }

        Console::warning("Khong tim thay ma hang. Vui long nhap lai.");
    }

    int iSoLuongMoi = 0;
    if (!this->docSoLuong(iSoLuongMoi))
    {
        return false;
    }

    if (!danhSach.capNhatSoLuongTheoMa(strMa, iSoLuongMoi))
    {
        Console::error("Cap nhat so luong that bai.");
        return false;
    }

    _output << '\n';
    Console::success("Cap nhat so luong thanh cong!");
    return true;
}