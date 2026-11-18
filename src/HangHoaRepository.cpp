#include "HangHoaRepository.h"

const char* HangHoaRepository::duoiFileTam()
{
    return ".tmp";
}

const char* HangHoaRepository::duoiFileBackup()
{
    return ".bak";
}

std::string HangHoaRepository::trim(const std::string& strInput)
{
    const std::string::size_type iDau = strInput.find_first_not_of(" \t\r\n");
    if (iDau == std::string::npos)
    {
        return std::string();
    }

    const std::string::size_type iCuoi = strInput.find_last_not_of(" \t\r\n");
    return strInput.substr(iDau, iCuoi - iDau + 1);
}

bool HangHoaRepository::laSoThucHopLe(const std::string& strInput, double& dKetQua)
{
    std::istringstream streamNhap(HangHoaRepository::trim(strInput));
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

bool HangHoaRepository::laSoNguyenHopLe(const std::string& strInput, int& iKetQua)
{
    std::istringstream streamNhap(HangHoaRepository::trim(strInput));
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

bool HangHoaRepository::truongDuLieuHopLe(const std::string& strDuLieu)
{
    return strDuLieu.find('|') == std::string::npos &&
           strDuLieu.find('\n') == std::string::npos &&
           strDuLieu.find('\r') == std::string::npos;
}

bool HangHoaRepository::parseDong(const std::string& strDong, HangHoa& hangHoa)
{
    std::istringstream streamDong(strDong);
    std::string strMa;
    std::string strTen;
    std::string strNoiSanXuat;
    std::string strMauSac;
    std::string strGia;
    std::string strNgayNhap;
    std::string strSoLuong;

    if (!std::getline(streamDong, strMa, '|') ||
        !std::getline(streamDong, strTen, '|') ||
        !std::getline(streamDong, strNoiSanXuat, '|') ||
        !std::getline(streamDong, strMauSac, '|') ||
        !std::getline(streamDong, strGia, '|') ||
        !std::getline(streamDong, strNgayNhap, '|') ||
        !std::getline(streamDong, strSoLuong))
    {
        return false;
    }

    double dGiaBan = 0.0;
    int iSoLuong = 0;

    strMa = DanhSachHangHoa::chuanHoaMaHang(strMa);
    if (!DanhSachHangHoa::maHangHopLe(strMa) ||
        !HangHoaRepository::laSoThucHopLe(strGia, dGiaBan) ||
        !HangHoaRepository::laSoNguyenHopLe(strSoLuong, iSoLuong) ||
        !HangHoa::ngayHopLe(HangHoaRepository::trim(strNgayNhap)) ||
        !HangHoaRepository::truongDuLieuHopLe(strTen) ||
        !HangHoaRepository::truongDuLieuHopLe(strNoiSanXuat) ||
        !HangHoaRepository::truongDuLieuHopLe(strMauSac) ||
        !HangHoaRepository::truongDuLieuHopLe(strNgayNhap))
    {
        return false;
    }

    hangHoa = HangHoa(
        strMa,
        HangHoaRepository::trim(strTen),
        HangHoaRepository::trim(strNoiSanXuat),
        HangHoaRepository::trim(strMauSac),
        dGiaBan,
        HangHoaRepository::trim(strNgayNhap),
        iSoLuong
    );

    return true;
}

bool HangHoaRepository::fileTonTai(const std::string& strDuongDan) const
{
    std::ifstream fileIn(strDuongDan.c_str(), std::ios::binary);
    return fileIn.good();
}

bool HangHoaRepository::docTuDuongDan(
    const std::string& strDuongDan,
    DanhSachHangHoa& danhSach,
    std::string& strLoi
) const
{
    std::ifstream fileIn(strDuongDan.c_str(), std::ios::in | std::ios::binary);
    if (!fileIn.is_open())
    {
        strLoi = "Khong mo duoc tep: " + strDuongDan;
        return false;
    }

    DanhSachHangHoa danhSachTam;
    std::string strDong;
    std::size_t iSoDong = 0;
    std::size_t iSoDongLoi = 0;
    std::string strLoiDauTien;

    while (std::getline(fileIn, strDong))
    {
        ++iSoDong;
        if (HangHoaRepository::trim(strDong).empty())
        {
            continue;
        }

        HangHoa hangHoa;
        if (!HangHoaRepository::parseDong(strDong, hangHoa))
        {
            if (iSoDongLoi == 0)
            {
                std::ostringstream streamLoi;
                streamLoi << "Dong " << iSoDong << " khong hop le";
                strLoiDauTien = streamLoi.str();
            }
            ++iSoDongLoi;
            continue;
        }

        if (!danhSachTam.them(hangHoa))
        {
            if (iSoDongLoi == 0)
            {
                std::ostringstream streamLoi;
                streamLoi << "Ma hang bi trung tai dong " << iSoDong;
                strLoiDauTien = streamLoi.str();
            }
            ++iSoDongLoi;
        }
    }

    if (fileIn.bad())
    {
        strLoi = "Loi trong qua trinh doc tep: " + strDuongDan;
        return false;
    }

    danhSach.swap(danhSachTam);
    if (iSoDongLoi == 0)
    {
        strLoi.clear();
    }
    else
    {
        std::ostringstream streamLoi;
        streamLoi << "Da bo qua " << iSoDongLoi << " dong loi trong tep hang hoa ("
                  << strLoiDauTien << ").";
        strLoi = streamLoi.str();
    }
    return true;
}

bool HangHoaRepository::khoiPhucTepChinhTu(const std::string& strNguon, std::string& strLoi) const
{
    const std::string strTepKhoiPhucTam = this->_strDuongDanFile + ".restore.tmp";
    std::ifstream fileNguon(strNguon.c_str(), std::ios::in | std::ios::binary);
    if (!fileNguon.is_open())
    {
        strLoi = "Khong mo duoc tep phuc hoi: " + strNguon;
        return false;
    }

    std::ofstream fileTam(
        strTepKhoiPhucTam.c_str(),
        std::ios::out | std::ios::trunc | std::ios::binary
    );
    if (!fileTam.is_open())
    {
        strLoi = "Khong tao duoc tep tam de phuc hoi du lieu.";
        return false;
    }

    fileTam << fileNguon.rdbuf();
    fileTam.flush();
    const bool bSaoChepThanhCong = !fileNguon.bad() && static_cast<bool>(fileTam);
    fileNguon.close();
    fileTam.close();

    if (!bSaoChepThanhCong || !fileTam)
    {
        std::remove(strTepKhoiPhucTam.c_str());
        strLoi = "Sao chep tep phuc hoi that bai.";
        return false;
    }

    if (std::rename(strTepKhoiPhucTam.c_str(), this->_strDuongDanFile.c_str()) != 0)
    {
        std::remove(strTepKhoiPhucTam.c_str());
        strLoi = "Khong the dat tep phuc hoi vao vi tri tep chinh.";
        return false;
    }

    return true;
}

HangHoaRepository::HangHoaRepository()
    : _strDuongDanFile("data/HangHoa.txt")
{
}

HangHoaRepository::HangHoaRepository(const std::string& strDuongDanFile)
    : _strDuongDanFile(strDuongDanFile)
{
}

bool HangHoaRepository::docFile(DanhSachHangHoa& danhSach, std::string& strLoi) const
{
    strLoi.clear();

    // Neu tep chinh con ton tai, chi doc tep chinh va khong ghi de len du lieu loi.
    if (this->fileTonTai(this->_strDuongDanFile))
    {
        DanhSachHangHoa danhSachTam;
        if (!this->docTuDuongDan(this->_strDuongDanFile, danhSachTam, strLoi))
        {
            return false;
        }

        danhSach.swap(danhSachTam);
        return true;
    }

    // Tep chinh co the bi mat neu chuong trinh dung dot ngot giua hai lenh rename.
    // Uu tien tep .swap (ban goc ngay truoc khi commit), sau do moi den backup.
    const std::string dsTepPhucHoi[] =
    {
        this->_strDuongDanFile + ".swap",
        this->_strDuongDanFile + HangHoaRepository::duoiFileBackup(),
        this->_strDuongDanFile + HangHoaRepository::duoiFileBackup() + ".tmp"
    };

    for (std::size_t i = 0; i < sizeof(dsTepPhucHoi) / sizeof(dsTepPhucHoi[0]); ++i)
    {
        if (!this->fileTonTai(dsTepPhucHoi[i]))
        {
            continue;
        }

        DanhSachHangHoa danhSachPhucHoi;
        std::string strLoiPhucHoi;
        if (!this->docTuDuongDan(dsTepPhucHoi[i], danhSachPhucHoi, strLoiPhucHoi))
        {
            continue;
        }

        if (!this->khoiPhucTepChinhTu(dsTepPhucHoi[i], strLoi))
        {
            return false;
        }

        danhSach.swap(danhSachPhucHoi);
        strLoi = "Tep du lieu chinh bi thieu; da khoi phuc tu " + dsTepPhucHoi[i] + ".";
        return true;
    }

    strLoi = "Khong tim thay tep du lieu hang hoa hop le: " + this->_strDuongDanFile;
    return false;
}

bool HangHoaRepository::ghiFile(const DanhSachHangHoa& danhSach, std::string& strLoi) const
{
    strLoi.clear();

    if (!this->fileTonTai(this->_strDuongDanFile))
    {
        strLoi = "Tep du lieu goc khong ton tai; tu choi tao moi de tranh ghi de nham.";
        return false;
    }

    const std::string strFileTam = this->_strDuongDanFile + HangHoaRepository::duoiFileTam();
    const std::string strFileBackup = this->_strDuongDanFile + HangHoaRepository::duoiFileBackup();
    const std::string strFileBackupTam = strFileBackup + ".tmp";
    const std::string strFileHoanDoi = this->_strDuongDanFile + ".swap";

    // Buoc 1: ghi toan bo du lieu moi vao tep tam, khong cham vao tep goc.
    {
        std::ofstream fileOut(
            strFileTam.c_str(),
            std::ios::out | std::ios::trunc | std::ios::binary
        );

        if (!fileOut.is_open())
        {
            strLoi = "Khong tao duoc tep tam de luu hang hoa.";
            return false;
        }

        const NODE* pCurrent = danhSach.getHead();
        while (pCurrent != NULL)
        {
            const HangHoa& hangHoa = pCurrent->Data;

            if (!DanhSachHangHoa::maHangHopLe(hangHoa.getMaHang()) ||
                !HangHoaRepository::truongDuLieuHopLe(hangHoa.getTenHang()) ||
                !HangHoaRepository::truongDuLieuHopLe(hangHoa.getNoiSanXuat()) ||
                !HangHoaRepository::truongDuLieuHopLe(hangHoa.getMauSac()) ||
                !HangHoaRepository::truongDuLieuHopLe(hangHoa.getNgayNhapKho()) ||
                !std::isfinite(hangHoa.getGiaBan()) || hangHoa.getGiaBan() <= 0.0 ||
                !HangHoa::ngayHopLe(hangHoa.getNgayNhapKho()) ||
                hangHoa.getSoLuong() < 0)
            {
                fileOut.close();
                std::remove(strFileTam.c_str());
                strLoi = "Thong tin hang hoa khong hop le hoac chua ky tu phan tach.";
                return false;
            }

            fileOut << hangHoa.getMaHang() << '|'
                    << hangHoa.getTenHang() << '|'
                    << hangHoa.getNoiSanXuat() << '|'
                    << hangHoa.getMauSac() << '|'
                    << std::setprecision(std::numeric_limits<double>::max_digits10)
                    << hangHoa.getGiaBan() << '|'
                    << hangHoa.getNgayNhapKho() << '|'
                    << hangHoa.getSoLuong() << '\n';

            if (!fileOut)
            {
                fileOut.close();
                std::remove(strFileTam.c_str());
                strLoi = "Loi khi ghi du lieu vao tep tam.";
                return false;
            }

            pCurrent = pCurrent->Next;
        }

        fileOut.flush();
        if (!fileOut)
        {
            fileOut.close();
            std::remove(strFileTam.c_str());
            strLoi = "Khong the flush du lieu vao tep tam.";
            return false;
        }

        fileOut.close();
        if (!fileOut)
        {
            std::remove(strFileTam.c_str());
            strLoi = "Khong the dong tep tam sau khi ghi.";
            return false;
        }
    }

    // Buoc 2: sao chep tep hien tai sang backup tam. Backup cu van duoc giu.
    {
        std::ifstream fileGoc(this->_strDuongDanFile.c_str(), std::ios::in | std::ios::binary);
        std::ofstream fileBackupTam(
            strFileBackupTam.c_str(),
            std::ios::out | std::ios::trunc | std::ios::binary
        );

        if (!fileGoc.is_open() || !fileBackupTam.is_open())
        {
            fileBackupTam.close();
            std::remove(strFileTam.c_str());
            std::remove(strFileBackupTam.c_str());
            strLoi = "Khong tao duoc ban sao backup tam; tep goc duoc giu nguyen.";
            return false;
        }

        fileBackupTam << fileGoc.rdbuf();
        fileBackupTam.flush();
        const bool bSaoChepThanhCong = !fileGoc.bad() && static_cast<bool>(fileBackupTam);
        fileGoc.close();
        fileBackupTam.close();

        if (!bSaoChepThanhCong || !fileBackupTam)
        {
            std::remove(strFileTam.c_str());
            std::remove(strFileBackupTam.c_str());
            strLoi = "Sao chep backup that bai; tep goc duoc giu nguyen.";
            return false;
        }
    }

    // Xoa tep swap cu neu no con lai tu mot lan tat chuong trinh dot ngot.
    if (this->fileTonTai(strFileHoanDoi) && std::remove(strFileHoanDoi.c_str()) != 0)
    {
        std::remove(strFileTam.c_str());
        std::remove(strFileBackupTam.c_str());
        strLoi = "Khong the don dep tep swap cu; tep goc duoc giu nguyen.";
        return false;
    }

    // Buoc 3: chuyen tep goc sang tep swap, sau do dua tep moi vao vi tri chinh.
    if (std::rename(this->_strDuongDanFile.c_str(), strFileHoanDoi.c_str()) != 0)
    {
        std::remove(strFileTam.c_str());
        std::remove(strFileBackupTam.c_str());
        strLoi = "Khong the chuyen tep goc sang tep swap; du lieu goc duoc giu nguyen.";
        return false;
    }

    if (std::rename(strFileTam.c_str(), this->_strDuongDanFile.c_str()) != 0)
    {
        const bool bKhoiPhucThanhCong =
            (std::rename(strFileHoanDoi.c_str(), this->_strDuongDanFile.c_str()) == 0);
        std::remove(strFileTam.c_str());
        std::remove(strFileBackupTam.c_str());

        strLoi = bKhoiPhucThanhCong
            ? "Khong the thay tep moi; tep goc da duoc khoi phuc."
            : "Khong the thay tep moi; tep goc con trong tep .swap va ban backup tam.";
        return false;
    }

    // Buoc 4: tep moi da duoc dat thanh cong. Bay gio moi cap nhat ban .bak.
    if (this->fileTonTai(strFileBackup) && std::remove(strFileBackup.c_str()) != 0)
    {
        strLoi = "Da luu tep chinh, nhung khong the thay backup cu; tep .swap va .bak.tmp duoc giu lai.";
        return true;
    }

    if (std::rename(strFileBackupTam.c_str(), strFileBackup.c_str()) != 0)
    {
        strLoi = "Da luu tep chinh, nhung khong the dat ten backup .bak; tep .swap va .bak.tmp duoc giu lai.";
        return true;
    }

    // Backup .bak da co ban truoc khi thay doi; co the don tep swap.
    std::remove(strFileHoanDoi.c_str());
    return true;
}
