import time
import threading

class buffer():
    def __init__(self, size):  
        self.contents = []
        self.size = size
        self.front = 0
        self.back = size - 1
        self.mux = threading.Semaphore(1)
        self.toPut = threading.Semaphore(self.size)
        self.toGet = threading.Semaphore(0)

    def myput(self, item):
        self.toPut.acquire()
        self.mux.acquire()
            
        self.back += 1
        if self.back == size : self.back = 0 
        self.contents.insert(self.back, item) 
                 
        self.mux.release()
        self.toGet.release()

    def myget(self):
        self.toGet.acquire()
        self.mux.acquire()
            
        item = self.contents[self.front]
        self.front += 1
        if self.front == size : self.front = 0 
                    
        self.mux.release()
        self.toPut.release()
        return item    

def pro(tid, mybuffer, myloop, mydelay):
    for i in range(myloop): 
        print("Prod %d item %d" % (tid,i)) 
        mybuffer.myput(i)
        time.sleep(mydelay)       
        
def con(tid, mybuffer, myloop, mydelay):
    while True: 
        i = mybuffer.myget()
        print("Cons %d item %d" % (tid,i)) 
        time.sleep(mydelay)             

if __name__ == '__main__':
    size = 10
    loop = 10
    npro = 2
    ncon = 2
    dpro = 0.1
    dcon = 0.2
    
    a_buffer = buffer(size)
    pros = [threading.Thread(target=pro, args=(i, a_buffer, loop, dpro)) for i in range(npro)]
    cons = [threading.Thread(target=con, args=(i, a_buffer, loop, dcon)) for i in range(ncon)]
    
    for t in pros:
            t.start()
    for t in cons:
            t.start()
    for t in pros:
            t.join()
    for t in cons:
            t.join()        


