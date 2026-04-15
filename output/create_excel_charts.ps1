param(
    [string]$BaseDir = "."
)

$ErrorActionPreference = "Stop"

$basePath = (Resolve-Path $BaseDir).Path
$csvResumo = Join-Path $basePath "output\summary_by_family.csv"
$csvTop = Join-Path $basePath "output\top_makespan.csv"
$xlsxOut = Join-Path $basePath "output\charts_presentation.xlsx"

if (-not (Test-Path $csvResumo)) {
    throw "Arquivo nao encontrado: $csvResumo"
}
if (-not (Test-Path $csvTop)) {
    throw "Arquivo nao encontrado: $csvTop"
}

$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$excel.DisplayAlerts = $false

try {
    $wb = $excel.Workbooks.Add()

    # Remove planilhas extras para manter o arquivo limpo.
    while ($wb.Worksheets.Count -gt 1) {
        $wb.Worksheets.Item(2).Delete()
    }

    $wsResumo = $wb.Worksheets.Item(1)
    $wsResumo.Name = "ResumoFamilias"

    $wsTop = $wb.Worksheets.Add()
    $wsTop.Name = "TopExtremos"

    function Load-CsvToSheet {
        param(
            [Parameter(Mandatory = $true)]$Sheet,
            [Parameter(Mandatory = $true)][string]$CsvPath
        )

        $data = Import-Csv $CsvPath
        if ($data.Count -eq 0) {
            return
        }

        $headers = $data[0].PSObject.Properties.Name
        for ($c = 0; $c -lt $headers.Count; $c++) {
            $Sheet.Cells.Item(1, $c + 1) = $headers[$c]
        }

        for ($r = 0; $r -lt $data.Count; $r++) {
            $rowObj = $data[$r]
            for ($c = 0; $c -lt $headers.Count; $c++) {
                $value = $rowObj.($headers[$c])
                $Sheet.Cells.Item($r + 2, $c + 1) = $value
            }
        }

        $used = $Sheet.UsedRange
        $used.Columns.AutoFit() | Out-Null

        # Estilo simples de cabecalho.
        $headerRange = $Sheet.Range($Sheet.Cells.Item(1, 1), $Sheet.Cells.Item(1, $headers.Count))
        $headerRange.Font.Bold = $true
        $headerRange.Interior.Color = 0x00EDEDED
    }

    Load-CsvToSheet -Sheet $wsResumo -CsvPath $csvResumo
    Load-CsvToSheet -Sheet $wsTop -CsvPath $csvTop

    # 1) Grafico de barras: media de makespan por familia.
    $chartObj1 = $wsResumo.ChartObjects().Add(20, 220, 540, 260)
    $chart1 = $chartObj1.Chart
    $lastRowResumo = $wsResumo.UsedRange.Rows.Count
    $rangeCat1 = $wsResumo.Range("A2:A$lastRowResumo")
    $rangeVal1 = $wsResumo.Range("C2:C$lastRowResumo")

    $chart1.ChartType = 51  # xlColumnClustered
    $chart1.HasTitle = $true
    $chart1.ChartTitle.Text = "Media de Makespan por Familia"
    $chart1.SeriesCollection().NewSeries() | Out-Null
    $chart1.SeriesCollection(1).Name = "Media"
    $chart1.SeriesCollection(1).XValues = $rangeCat1
    $chart1.SeriesCollection(1).Values = $rangeVal1

    # 2) Grafico de barras: quantidade de instancias por familia.
    $chartObj2 = $wsResumo.ChartObjects().Add(580, 220, 540, 260)
    $chart2 = $chartObj2.Chart
    $rangeVal2 = $wsResumo.Range("B2:B$lastRowResumo")

    $chart2.ChartType = 51  # xlColumnClustered
    $chart2.HasTitle = $true
    $chart2.ChartTitle.Text = "Quantidade de Instancias por Familia"
    $chart2.SeriesCollection().NewSeries() | Out-Null
    $chart2.SeriesCollection(1).Name = "Quantidade"
    $chart2.SeriesCollection(1).XValues = $rangeCat1
    $chart2.SeriesCollection(1).Values = $rangeVal2

    # 3) Grafico de comparacao dos extremos (top5 maiores e menores).
    $chartObj3 = $wsTop.ChartObjects().Add(20, 220, 740, 300)
    $chart3 = $chartObj3.Chart
    $lastRowTop = $wsTop.UsedRange.Rows.Count
    $rangeCat3 = $wsTop.Range("B2:B$lastRowTop")
    $rangeVal3 = $wsTop.Range("E2:E$lastRowTop")

    $chart3.ChartType = 51  # xlColumnClustered
    $chart3.HasTitle = $true
    $chart3.ChartTitle.Text = "Top 5 Maiores e Top 5 Menores Makespans"
    $chart3.SeriesCollection().NewSeries() | Out-Null
    $chart3.SeriesCollection(1).Name = "Makespan"
    $chart3.SeriesCollection(1).XValues = $rangeCat3
    $chart3.SeriesCollection(1).Values = $rangeVal3

    # Salva em xlsx.
    $xlOpenXMLWorkbook = 51
    $wb.SaveAs($xlsxOut, $xlOpenXMLWorkbook)
    $wb.Close($true)

    Write-Output "Workbook gerado: $xlsxOut"
}
finally {
    $excel.Quit()
    [System.Runtime.InteropServices.Marshal]::ReleaseComObject($excel) | Out-Null
}
