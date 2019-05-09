import urllib.request


headers = {'User-Agent':'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:23.0) Gecko/20100101 Firefox/23.0'}
req = urllib.request.Request(url="https://pokemon.pokego2.com/posshiny-true", headers=headers)
txt = urllib.request.urlopen(req)
for line in txt:
	print(line)