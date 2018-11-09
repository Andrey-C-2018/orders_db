Cells.Select
Selection.Copy

Dim wb As Workbook
Set wb = Workbooks.Add(1)

wb.Sheets("Лист1").Select
wb.Sheets("Лист1").Name = "Техн_інформація"
wb.Sheets("Техн_інформація").Paste

Dim n As Name
For Each n In ThisWorkbook.Names
wb.Names.Add Name:=n.Name, RefersTo:=Replace(n.RefersTo, ThisWorkbook.Sheets(1).Name, wb.Sheets(1).Name)