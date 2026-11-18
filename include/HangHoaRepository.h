#pragma once

#include "DanhSachHangHoa.h"
#include "HangHoa.h"

#include <cstdio>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

class HangHoaRepository
{
private:
    std::string _strDuongDanFile;

    static const char* duoiFileTam();

    static const char* duoiFileBackup();

    static std::string trim(const std::string& strInput);

    static bool laSoThucHopLe(const std::string& strInput, double& dKetQua);

    static bool laSoNguyenHopLe(const std::string& strInput, int& iKetQua);

    static bool truongDuLieuHopLe(const std::string& strDuLieu);

    static bool parseDong(const std::string& strDong, HangHoa& hangHoa);

    bool fileTonTai(const std::string& strDuongDan) const;

    /**************************************************************************
     * @Description Doc tung dong vao danh sach tam; bo qua dong loi va bao loi doc tep
     *************************************************************************/
    bool docTuDuongDan(
        const std::string& strDuongDan,
        DanhSachHangHoa& danhSach,
        std::string& strLoi
    ) const;

    /**************************************************************************
     * @Description Khoi phuc tep chinh tu mot tep hop le ma khong lam mat tep nguon
     *************************************************************************/
    bool khoiPhucTepChinhTu(const std::string& strNguon, std::string& strLoi) const;

public:
    HangHoaRepository();

    explicit HangHoaRepository(const std::string& strDuongDanFile);

    /**************************************************************************
     * @Description Nap du lieu vao danh sach tam; chi thay danh sach that neu doc thanh cong
     *************************************************************************/
    bool docFile(DanhSachHangHoa& danhSach, std::string& strLoi) const;

    /**************************************************************************
     * @Description Ghi tep tam, luu ban backup, sau do moi thay the tep chinh
     *************************************************************************/
    bool ghiFile(const DanhSachHangHoa& danhSach, std::string& strLoi) const;
};