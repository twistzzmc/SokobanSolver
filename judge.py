#!/usr/bin/python3

from time import sleep
import sys

end = None

def gotoRC(r,c):
  print("\033[{};{}f".format(r+3,c+1),end='')
end

def cls():
  print("\033[2J",end='')
  gotoRC(0,0)
end

def readBoard(fname):
  f=open(fname)
  W,K = f.readline().strip('\n').split()
  W = int(W)
  K = int(K)

  t = []
  for _ in range(W):
    t.append(list(f.readline().strip('\n')))
  end
  return t
end

def printBoard(t):
  cls()
  W = len(t)
  K = len(t[0])
  for w in range(W):
    for k in range(K):
      print(t[w][k],end='')
    end
    print()
  end
end

def readSolution(fname):
  with open(fname) as f:
    return f.readline().strip('\n')
  end
end

def start(t):
  W = len(t)
  K = len(t[0])

  pola = []
  for r in range(W):
    for c in range(K):
      if t[r][c]=='*':
        pola.append((r,c))
        t[r][c]=' '
      elif t[r][c]=='-':
        r1,c1 =  r,c
      end
    end
  end
  return pola,r1,c1
end


def check(t,s,view=True):
  """zwraca:
     0 : rozwiązanie poprawne
     1 : błędny kod ruchu
     2 : wejście na ścianę
     3 : wejście na skrzynię
     4 : brak skrzyni do ciągnięcia
     5 : nieustawione skrzynie
  """
  W = len(t)
  K = len(t[0])

  if view: printBoard(t)

  pola,r,c = start(t)
  licz = 0
  for z in s:
    licz+=1
    if view: gotoRC(-2,0); print(licz,":",r,c,z)
    r1,c1 = r,c
    if z=='l':
      c1-=1
      t[r][c]=' '
    elif z=='r':
      c1+=1
      t[r][c]=' '
    elif z=='u':
      r1-=1
      t[r][c]=' '
    elif z=='d':
      r1+=1
      t[r][c]=' '

    elif z=='L':
      if t[r][c+1]!='O': return 4
      c1-=1
      t[r][c+1]=' '
      if view: gotoRC(r,c+1);  print(' ',end='')
      t[r][c]='O'
    elif z=='R':
      if t[r][c-1]!='O': return 4
      c1+=1
      t[r][c-1]=' '
      if view: gotoRC(r,c-1);  print(' ',end='')
      t[r][c]='O'
    elif z=='U':
      if t[r+1][c]!='O': return 4
      r1-=1
      t[r+1][c]=' '
      if view: gotoRC(r+1,c);  print(' ',end='')
      t[r][c]='O'
    elif z=='D':
      if t[r-1][c]!='O': return 4
      r1+=1
      t[r-1][c]=' '
      if view: gotoRC(r-1,c);  print(' ',end='')
      t[r][c]='O'
    else: return 1

    if t[r1][c1]=='#': return 2
    if t[r1][c1]=='O': return 3

    t[r1][c1]='-'
    
    if view: 
      gotoRC(r,c);    print(t[r][c],end='')
      gotoRC(r1,c1);  print('-',end='')
      for (y,x) in pola:
        if t[y][x] != "O":
          gotoRC(y,x)
          print('*',end='')

    end

    r,c = r1,c1

    if view: sleep(0.1)
  end

  for r,c in pola:
    if t[r][c]!='O':
      return 5
    end
  end
  return 0
end

if __name__=="__main__":
  if len(sys.argv)<3:
    print("Wywołanie:\n   judge.py <infile> <outfile> [-v]")
  else:
    view = len(sys.argv)==4 and sys.argv[3]=='-v'

    t = readBoard(sys.argv[1])
    s = readSolution(sys.argv[2])

    ret = check(t,s,view)
    if view: gotoRC(-2,20)

    if ret==0:
      print('OK')
      exit(0)
    else:
      print('ERROR',ret)
      exit(1)
    end
  end
end
