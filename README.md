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
