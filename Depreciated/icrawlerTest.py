from icrawler.builtin import GoogleImageCrawler

e = GoogleImageCrawler(storage = {'root_dir': r'~/Downloads'})
e.crawl(keyword = 'sad carrots', max_num = 800)
