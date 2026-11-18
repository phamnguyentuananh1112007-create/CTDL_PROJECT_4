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
    // Dung chung mot bo kiem tra de tranh giao dien va lop du lieu
    // cho ra ket qua khac nhau.
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
            _output << "Thong tin khong duoc de trong. Vui long nhap lai.\n";
            continue;
        }

        if (strKetQua.find('|') != std::string::npos)
        {
            _output << "Thong tin khong duoc chua ky tu '|'. Vui long nhap lai.\n";
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

        _output << "Gia ban khong hop le. Gia ban bat buoc phai lon hon 0. Vui long nhap lai.\n";
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

        _output << "Ngay khong hop le. Vui long nhap theo dd/MM/yyyy "
                << "va dam bao ngay ton tai tren lich.\n";
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

        _output << "So luong khong hop le. Vui long nhap so nguyen >= 0.\n";
    }
}

void HangHoaConsole::inTieuDeBang() const
{
    _output << std::left
            << std::setw(COL_WIDTH_MA) << "Ma"
            << std::setw(COL_WIDTH_TEN) << "Ten Hang"
            << std::setw(COL_WIDTH_NOI_SX) << "Noi SX"
            << std::setw(COL_WIDTH_MAU) << "Mau"
            << std::setw(COL_WIDTH_GIA) << "Gia ban"
            << std::setw(COL_WIDTH_NGAY) << "Ngay nhap"
            << std::setw(COL_WIDTH_SL) << "So luong"
            << '\n';

    const std::string strDuongKe(
        static_cast<std::string::size_type>(
            COL_WIDTH_MA + COL_WIDTH_TEN + COL_WIDTH_NOI_SX +
            COL_WIDTH_MAU + COL_WIDTH_GIA + COL_WIDTH_NGAY + COL_WIDTH_SL
        ),
        '-'
    );
    _output << strDuongKe << '\n';
}

void HangHoaConsole::inMotHangHoa(const HangHoa& hangHoa) const
{
    _output << std::left
            << std::setw(COL_WIDTH_MA) << hangHoa.getMaHang()
            << std::setw(COL_WIDTH_TEN) << hangHoa.getTenHang()
            << std::setw(COL_WIDTH_NOI_SX) << hangHoa.getNoiSanXuat()
            << std::setw(COL_WIDTH_MAU) << hangHoa.getMauSac()
            << std::setw(COL_WIDTH_GIA) << std::fixed << std::setprecision(0)
            << hangHoa.getGiaBan()
            << std::setw(COL_WIDTH_NGAY) << hangHoa.getNgayNhapKho()
            << std::setw(COL_WIDTH_SL) << hangHoa.getSoLuong()
            << '\n';
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
        }

        pCurrent = pCurrent->Next;
    }

    if (!bDaInTieuDe)
    {
        _output << (chiHangDangBan
            ? "Hien khong co hang hoa nao dang ban.\n"
            : "Danh sach hang hoa rong!\n");
    }
}

void HangHoaConsole::hienThiSapXep(
    const DanhSachHangHoa& danhSach,
    const bool bTangDan,
    const bool chiHangDangBan) const
{
    std::vector<HangHoa> dsSapXep;
    const NODE* pCurrent = danhSach.getHead();

    // Chuyen cac hang hoa can hien thi tu danh sach lien ket sang vector.
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
        _output << (chiHangDangBan
            ? "Hien khong co hang hoa nao dang ban.\n"
            : "Danh sach hang hoa rong!\n");
        return;
    }

    // Su dung Bubble Sort duoc dinh nghia trong HangHoaSapXep.h.
    HangHoaSapXep::bubbleSort(dsSapXep, bTangDan);

    _output << (bTangDan
        ? "\nDanh sach hang hoa theo gia tu thap den cao:\n"
        : "\nDanh sach hang hoa theo gia tu cao den thap:\n");
    this->inTieuDeBang();

    for (std::vector<HangHoa>::const_iterator it = dsSapXep.begin();
         it != dsSapXep.end(); ++it)
    {
        this->inMotHangHoa(*it);
    }
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
        _output << "Tu khoa tim kiem khong duoc de trong.\n";
        return;
    }

    bool bDaInTieuDe = false;
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
                _output << "\nCac hang hoa dang ban tim thay:\n";
                this->inTieuDeBang();
                bDaInTieuDe = true;
            }

            this->inMotHangHoa(hangHoa);
        }

        pCurrent = pCurrent->Next;
    }

    if (!bDaInTieuDe)
    {
        _output << "Khong tim thay hang hoa dang ban phu hop!\n";
    }
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
            _output << "Da huy them hang hoa.\n";
            return false;
        }

        if (!DanhSachHangHoa::maHangHopLe(strMa))
        {
            _output << "Ma hang phai co dung "
                    << DanhSachHangHoa::doDaiMaHangBatBuoc()
                    << " ky tu. Vui long nhap lai.\n";
            continue;
        }

        if (danhSach.kiemTraMa(strMa))
        {
            _output << "Ma hang da ton tai. Vui long nhap ma khac.\n";
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
        _output << "Da huy thao tac do ket thuc dau vao.\n";
        return false;
    }

    const HangHoa hangHoaMoi(
        strMa, strTenHang, strNoiSanXuat, strMauSac,
        dGiaBan, strNgayNhapKho, iSoLuong
    );

    if (!hangHoaMoi.duLieuHopLe())
    {
        _output << "Khong the them hang hoa: gia ban phai > 0 va ngay nhap kho phai la ngay hop le.\n";
        return false;
    }

    if (!danhSach.them(hangHoaMoi))
    {
        _output << "Khong the them hang hoa. Ma hang co the da ton tai.\n";
        return false;
    }

    _output << "Them hang hoa thanh cong!\n";
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
            _output << "Da huy xoa hang hoa.\n";
            return false;
        }

        if (danhSach.kiemTraMa(strMa))
        {
            if (kiemTraDangCho && kiemTraDangCho(strMa))
            {
                _output << "Khong the xoa ma hang " << strMa
                        << " vi van con don hang dang cho xu ly tham chieu den mat hang nay.\n";
                return false;
            }
            break;
        }

        _output << "Khong tim thay ma hang. Vui long nhap lai.\n";
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
            _output << "Da huy cap nhat so luong.\n";
            return false;
        }

        if (danhSach.kiemTraMa(strMa))
        {
            break;
        }

        _output << "Khong tim thay ma hang. Vui long nhap lai.\n";
    }

    int iSoLuongMoi = 0;
    if (!this->docSoLuong(iSoLuongMoi))
    {
        return false;
    }

    if (!danhSach.capNhatSoLuongTheoMa(strMa, iSoLuongMoi))
    {
        _output << "Cap nhat so luong that bai.\n";
        return false;
    }

    _output << "Cap nhat so luong thanh cong!\n";
    return true;
}
