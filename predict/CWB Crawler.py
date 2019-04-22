# %%
# -*- coding: utf-8 -*-
import pandas as pd
import datetime
from datetime import timedelta
import time
import numpy as np


coding = 'utf-8'
# date為抓取的日期
date = datetime.date(2019, 2, 28)
dateStr = date.strftime('%Y-%m-%d')
dateStr
zero = '00'
url = 'https://e-service.cwb.gov.tw/HistoryDataQuery/DayDataController.do?command=viewMain&station=466900&stname=%25E6%25B7%25A1%25E6%25B0%25B4&datepicker='+dateStr


# 讀取氣象局觀測資料網頁，並輸入至DataFrame中
table = pd.read_html(url,encoding=coding,index_col=0,header=None,flavor='bs4')[1]



# 移除非數值資料
table.replace('...', 'NaN', inplace=True)
table.replace('X', 'NaN', inplace=True)
table.replace('T', 'NaN', inplace=True)
table

# 時間欄位處理的程式，未完成有錯誤
#table[0].replace(['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24'], [dateStr+' '+'01'+':'+zero+':'+zero, dateStr+' '+'02'+':'+zero+':'+zero, dateStr+' '+'03'+':'+zero+':'+zero, dateStr+' '+'04'+':'+zero+':'+zero, dateStr+' '+'05'+':'+zero+':'+zero, dateStr+' '+'06'+':'+zero+':'+zero, dateStr+' '+'07'+':'+zero+':'+zero, dateStr+' '+'08'+':'+zero+':'+zero, dateStr+' '+'09'+':'+zero+':'+zero, dateStr+' '+'10'+':'+zero +':'+zero, dateStr+' '+'11'+':'+zero+':'+zero, dateStr+' '+'12'+':'+zero+':'+zero, dateStr+' '+'13'+':'+zero+':'+zero, dateStr+' '+'14'+':'+zero+':'+zero, dateStr+' '+'15'+':'+zero+':'+zero, dateStr+' '+'16'+':'+zero+':'+zero, dateStr+' '+'17'+':'+zero+':'+zero, dateStr+' '+'18'+':'+zero+':'+zero, dateStr+' '+'19'+':'+zero+':'+zero, dateStr+' '+'20'+':'+zero+':'+zero, dateStr+' '+'21'+':'+zero+':'+zero, dateStr+' '+'22'+':'+zero+':'+zero, dateStr+' '+'23'+':'+zero+':'+zero, str(date+timedelta(days=1))+' '+zero+':'+zero+':'+zero], inplace=True)



