from PIL import Image, ImageDraw, ImageFont, ImageFilter
from pathlib import Path
import shutil,math,wave,struct
root=Path(__file__).parent/'assets';root.mkdir(exist_ok=True)
font='/usr/share/fonts/opentype/noto/NotoSerifCJK-Bold.ttc'
if not Path(font).exists(): font='/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf'
shutil.copy('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',root/'font.ttf')
W,H=800,900
im=Image.new('RGB',(W,H));px=im.load()
for y in range(H):
 for x in range(W):
  grain=math.sin(x*.041+y*.013)*2+math.sin(x*.012-y*.037)*3
  px[x,y]=(int(222+grain-15*y/H),int(186+grain-16*y/H),int(122+grain-12*y/H))
d=ImageDraw.Draw(im,'RGBA')
d.rounded_rectangle((21,21,779,879),radius=19,outline=(96,58,29,245),width=12)
d.rounded_rectangle((43,42,757,857),radius=7,outline=(121,78,37,200),width=4)
# board line art, polished wooden board as a single baked image
ink=(97,60,30,235)
for c in range(9):
 x=80+c*80
 if c in (0,8):d.line((x,90,x,810),fill=ink,width=3)
 else:
  d.line((x,90,x,410),fill=ink,width=2)
  d.line((x,490,x,810),fill=ink,width=2)
for r in range(10):
 y=90+r*80;d.line((80,y,720,y),fill=ink,width=3 if r in (0,9) else 2)
d.line((320,90,480,250),fill=ink,width=2);d.line((480,90,320,250),fill=ink,width=2)
d.line((320,650,480,810),fill=ink,width=2);d.line((480,650,320,810),fill=ink,width=2)
riverfont=ImageFont.truetype(font,38)
d.text((239,437),'楚 河',font=riverfont,fill=(103,63,35,220),stroke_width=0)
d.text((467,437),'漢 界',font=riverfont,fill=(103,63,35,220),stroke_width=0)
# coordinates and position markers
coordfont=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',18)
for c in range(9):
 letter=chr(65+c);d.text((80+c*80,833),letter,font=coordfont,fill=(85,55,29),anchor='mm')
for r in range(10):d.text((53,90+r*80),str(10-r),font=coordfont,fill=(85,55,29),anchor='mm')
for r in (2,3,6,7):
 cs=(1,7) if r in (2,7) else (0,2,4,6,8)
 for c in cs:
  x,y=80+c*80,90+r*80
  for dx in (-1,1):
   if c==0 and dx==-1 or c==8 and dx==1:continue
   for dy in (-1,1):
    d.line((x+dx*8,y+dy*4,x+dx*8,y+dy*16),fill=ink,width=2)
    d.line((x+dx*8,y+dy*4,x+dx*20,y+dy*4),fill=ink,width=2)
im.save(root/'board.png',optimize=True)
chars={'general':('帥','將'),'advisor':('仕','士'),'elephant':('相','象'),'horse':('傌','馬'),'rook':('俥','車'),'cannon':('炮','砲'),'soldier':('兵','卒')}
pfont=ImageFont.truetype(font,43)
for name,syms in chars.items():
 for idx,team in enumerate(('red','black')):
  layer=Image.new('RGBA',(72,72),(0,0,0,0));a=ImageDraw.Draw(layer)
  a.ellipse((5,6,69,70),fill=(50,32,18,62))
  a.ellipse((2,1,67,66),fill=(245,215,156,255),outline=(89,49,24,255),width=4)
  a.ellipse((8,7,61,60),outline=(163,120,72,255),width=2)
  color=(172,42,34) if team=='red' else (39,47,48)
  a.text((35,33),syms[idx],font=pfont,anchor='mm',fill=color,stroke_width=0)
  layer.save(root/(team+'_'+name+'.png'))
for name,freq in [('move',460),('capture',230)]:
 sr=22050;dur=.12 if name=='move' else .24
 samples=[]
 for k in range(int(sr*dur)):
  t=k/sr;envelope=(1-t/dur)**2
  v=.25*envelope*(math.sin(2*math.pi*freq*t)+.23*math.sin(2*math.pi*freq*2.1*t))
  samples.append(struct.pack('<h',max(-32767,min(32767,int(v*32767)))))
 with wave.open(str(root/(name+'.wav')),'wb') as f:
  f.setnchannels(1);f.setsampwidth(2);f.setframerate(sr);f.writeframes(b''.join(samples))
print('Created assets:',len(list(root.iterdir())))
