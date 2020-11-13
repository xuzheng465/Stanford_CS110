
def eliminate_news_logging(str):
   _, prompt, results = str.partition("Enter a search term [or just hit <enter> to quit]:")
   if prompt == '': # If the prompt isn't found, just return the unfiltered stuff
      return str
   return results
