from pyngrok import ngrok
import sys
import time

print("Starting ngrok tunnel for public access...")
print("\nIMPORTANT: If this is your first time using ngrok:")
print("1. Visit https://dashboard.ngrok.com/auth/your-authtoken")
print("2. Copy your auth token")
print("3. Run: ngrok config add-authtoken YOUR_TOKEN")
print("   (or set it in the script below)\n")

auth_token = "YOUR_NGROK_AUTH_TOKEN"  # Replace with your token from https://dashboard.ngrok.com/auth/your-authtoken

if auth_token != "YOUR_NGROK_AUTH_TOKEN":
    ngrok.set_auth_token(auth_token)

try:
    public_url = ngrok.connect(5000)
    print(f"\n{'='*70}")
    print(f"✓ PUBLIC URL (Share this link with anyone):")
    print(f"  {public_url}")
    print(f"{'='*70}")
    print(f"\nLocal access:     http://127.0.0.1:5000")
    print(f"Network access:   http://192.168.1.12:5000")
    print(f"Public access:    {public_url}")
    print(f"\nPress CTRL+C to stop the tunnel.\n")
    
    ngrok_process = ngrok.get_ngrok_process()
    ngrok_process.proc.wait()
except Exception as e:
    print(f"Error: {e}")
    print("\nMake sure:")
    print("1. Your auth token is set correctly")
    print("2. ngrok is installed via: pip install pyngrok")
    sys.exit(1)
