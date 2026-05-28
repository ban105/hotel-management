# Hotel Management Console App

Ung dung quan ly khach san viet bang C, chay tren terminal/console.

## Yeu cau

- Windows
- GCC/MinGW da cai va co trong `PATH`
- PowerShell hoac terminal tuong duong

Kiem tra GCC:

```powershell
gcc --version
```

## Lay code ve may

```powershell
git clone https://github.com/ban105/hotel-management.git
cd hotel-management
```

## Build chuong trinh

Neu may co `make`:

```powershell
make
```

Neu khong co `make`, dung lenh GCC truc tiep:

```powershell
gcc -Wall -Wextra -std=c11 -Iinclude main.c src\room.c src\customer.c src\booking.c src\service.c src\bill.c src\admin.c src\employee.c src\menu.c src\utils.c -o hotel.exe
```

## Chay chuong trinh

```powershell
.\hotel.exe
```

## Tai khoan dang nhap

Chon vai tro tren man hinh dang nhap:

```text
1. Le tan
2. Admin
```

Mat khau mac dinh:

```text
Le tan: letan123
Admin : admin123
```

Tai khoan nen dung khi demo:

```text
Vai tro Le tan: chon 1, mat khau letan123
Vai tro Admin : chon 2, mat khau admin123
```

## Kich ban demo goi y

1. Dang nhap bang Le tan:
   - Chon `1. Le tan`.
   - Nhap mat khau `letan123`.

2. Them khach hang moi:
   - Vao `Quan ly Khach hang`.
   - Chon `Them khach hang moi`.
   - Nhap ho ten, so dien thoai, CCCD va hang thanh vien.
   - Ghi lai ma khach hang vua tao, vi du `KH001`.

3. Dat phong:
   - Vao `Dat phong / Check-in / Check-out`.
   - Chon `Dat phong moi`.
   - Nhap ma khach hang vua tao.
   - Nhap ngay check-in va check-out hop le.
   - Chon mot phong trong danh sach phong trong.
   - Xac nhan dat phong va ghi lai ma booking, vi du `BK001`.

4. Check-in:
   - Van trong menu dat phong.
   - Chon `Check-in`.
   - Nhap ma booking vua tao.
   - Xac nhan check-in.

5. Goi dich vu:
   - Quay lai menu chinh.
   - Vao `Quan ly Dich vu`.
   - Chon `Goi dich vu cho phong`.
   - Nhap ma booking dang check-in.
   - Chon mot hoac nhieu ma dich vu, nhap so luong, sau do nhap `0` de ket thuc.
   - Xac nhan luu dich vu.

6. Check-out:
   - Quay lai menu `Dat phong / Check-in / Check-out`.
   - Chon `Check-out`.
   - Nhap ma booking.
   - Xac nhan check-out.

7. Xuat hoa don:
   - Quay lai menu chinh.
   - Vao `Quan ly Hoa don`.
   - Chon `Xem / In hoa don theo booking`.
   - Nhap ma booking da check-out.
   - Chon `y` khi chuong trinh hoi xuat hoa don va cap nhat doanh thu.
   - File hoa don se duoc tao trong thu muc `output/`.

8. Xem bao cao:
   - Dang nhap bang Admin neu dang o vai tro Le tan.
   - Chon `Khu vuc Quan tri vien`.
   - Xem `Bao cao doanh thu tong hop`.
   - Xem `Thong ke hieu suat dich vu`.
   - Xem `Doanh thu theo khoang thoi gian` bang cach nhap khoang ngay co hoa don vua tao.

## Cau truc thu muc

```text
include/      Header files
src/          Source files
data/         Du lieu mau dang CSV
output/       Hoa don xuat ra tu chuong trinh
Makefile      Lenh build neu may co make
main.c        Diem vao chuong trinh
```

## Ghi chu

- Du lieu chinh nam trong thu muc `data/`.
- File doanh thu dung cho bao cao la `data/bills.txt`.
- Khong can commit file `.exe`; moi may nen tu build lai bang GCC.
- File `hotel.exe` da duoc bo qua trong `.gitignore`.
