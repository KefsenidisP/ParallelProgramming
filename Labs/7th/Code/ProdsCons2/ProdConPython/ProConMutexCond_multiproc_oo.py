import time
import multiprocessing

class buffer():
    def __init__(self, size): 
        manager = multiprocessing.Manager() 
        self.contents = manager.list()
        self.size = size
        self.front = multiprocessing.Value('i', 0)
        self.back = multiprocessing.Value('i', size - 1)
        self.counter = multiprocessing.Value('i', 0)
        self.lock = multiprocessing.Lock()
        self.Fullcond = multiprocessing.Condition(self.lock)
        self.Emptycond = multiprocessing.Condition(self.lock)

    def myput(self, item):
        self.Fullcond.acquire()
        try:
            while self.counter.value == size: 
                self.Fullcond.wait()
                
            self.back.value = self.back.value + 1
            if self.back.value == size : self.back.value = 0 
            self.contents.insert(self.back.value, item) 
            self.counter.value = self.counter.value + 1
          
            self.Emptycond.notify_all()
        finally:
            self.Fullcond.release()

    def myget(self):
        self.Emptycond.acquire()
        try:
            while self.counter.value == 0: 
                self.Emptycond.wait()
            
            item = self.contents[self.front.value]
            self.front.value = self.front.value + 1
            if self.front.value == size : self.front.value = 0 
            self.counter.value = self.counter.value - 1
            
            self.Fullcond.notify_all()
        finally:
            self.Emptycond.release()
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
    pros = [multiprocessing.Process(target=pro, args=(i, a_buffer, loop, dpro)) for i in range(npro)]
    cons = [multiprocessing.Process(target=con, args=(i, a_buffer, loop, dcon)) for i in range(ncon)]
    
    for t in pros:
            t.start()
    for t in cons:
            t.start()
    for t in pros:
            t.join()
    for t in cons:
            t.join()        

   

