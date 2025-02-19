from selenium import webdriver
from selenium.webdriver.common.by import By
import time
import requests
import os

def download_first_image(search_term):
    # Set up the Selenium WebDriver (for Chrome in this case)
    driver = webdriver.Chrome()  # Make sure ChromeDriver is installed and in PATH
    driver.get(f"https://duckduckgo.com/?q={search_term}&t=h_&iar=images&iax=images&ia=images")
    
    
    # Allow some time for images to load
    time.sleep(2)

    # Find the first image element
    try:
        images = driver.find_elements(By.CLASS_NAME, 'tile--img__img')
        if images:
            first_image = images[0]
            img_url = first_image.get_attribute('src')

            # If the image URL is valid, download the image
            if img_url and img_url.startswith('http'):
                img_response = requests.get(img_url)
                if img_response.status_code == 200:
                    file_path = os.path.join(os.getcwd(), search_term + '.jpg')
                    with open(file_path, 'wb') as f:
                        f.write(img_response.content)
                    print(f"Image saved to {file_path}")
                else:
                    print("Failed to download image.")
            else:
                print("Image URL not valid.")
        else:
            print("No images found.")
    finally:
        # Close the browser after completing the task
        driver.quit()

# Example usage
search_term = "benits"
download_first_image(search_term)

terms = [ "beans" , "bananas", "leeks", "potatoes", "macrel in tomato sauce"]
for i in range(0, len(terms)):
    download_first_image(terms[i])
