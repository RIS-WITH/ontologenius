import os

if os.environ["ROS_VERSION"] == "1":
    import rospy

    class OntoService :
        def __init__(self, srv_name, srv_type):
            self._client = rospy.ServiceProxy(srv_name, srv_type, True)
            self.srv_name = srv_name
            self.srv_type = srv_type

        def call(self, request, verbose):
            try:
                response = self._client(request)
                return response
            except (rospy.ServiceException, rospy.exceptions.TransportTerminated) as e:
                if verbose == True:
                    print("Failure to call " + self.srv_name)
                self._client = rospy.ServiceProxy(self.srv_name, self.srv_type, True)
                try:
                    response = self._client(request)
                    if verbose == True:
                        print("Restored " + self.srv_name)
                    return response
                except (rospy.ServiceException, rospy.exceptions.TransportTerminated) as e:
                    if verbose == True:
                        print("Failure of service restoration")
                    return None
    
    class OntoPublisher :
        def __init__(self, pub_name, pub_type, queue_size):
            self.pub = rospy.Publisher(pub_name, pub_type, queue_size=queue_size)

        def publish(self, msg):
            self.pub.publish(msg)
    
    class OntoSubscriber :
        def __init__(self, sub_name, sub_type, callback):
            self.sub = rospy.Subscriber(sub_name, sub_type, callback)

    class Ontoros :
        def createService(srv_name, srv_type):
            return OntoService(srv_name, srv_type)
        
        def createPublisher(pub_name, pub_type, queue_size):
            return OntoPublisher(pub_name, pub_type, queue_size)
        
        def createSubscriber(sub_name, sub_type, callback):
            return OntoSubscriber(sub_name, sub_type, callback)
        
        def getRosTime():
            return rospy.get_rostime()
        
        def isShutdown():
            return rospy.is_shutdown()

    print("v1")
elif os.environ["ROS_VERSION"] == "2":

    import rclpy
    from rclpy.node import Node
    from threading import Lock
    from rclpy.service import SrvTypeRequest, SrvTypeResponse
    from rclpy.client import Client
    from rclpy.publisher import Publisher
    from rclpy.subscription import Subscription
    from rclpy.time import Time


    class SingletonMeta(type):
        """
        This is a thread-safe implementation of Singleton.
        """

        _instances = {}

        _lock: Lock = Lock()
        """
        We now have a lock object that will be used to synchronize threads during
        first access to the Singleton.
        """

        def __call__(cls, *args, **kwargs):
            """
            Possible changes to the value of the `__init__` argument do not affect
            the returned instance.
            """
            # Now, imagine that the program has just been launched. Since there's no
            # Singleton instance yet, multiple threads can simultaneously pass the
            # previous conditional and reach this point almost at the same time. The
            # first of them will acquire lock and will proceed further, while the
            # rest will wait here.
            with cls._lock:
                # The first thread to acquire the lock, reaches this conditional,
                # goes inside and creates the Singleton instance. Once it leaves the
                # lock block, a thread that might have been waiting for the lock
                # release may then enter this section. But since the Singleton field
                # is already initialized, the thread won't create a new object.
                if cls not in cls._instances:
                    instance = super().__call__(*args, **kwargs)
                    cls._instances[cls] = instance
            return cls._instances[cls]


    print("v2")


    class OntoService:
        def __init__(self, client: Client, node_: Node):
            self.client: Client = client
            self.node_: Node = node_

        def call(self, params: SrvTypeRequest) -> SrvTypeResponse:
            future = self.client.call_async(request=params)
            rclpy.spin_until_future_complete(self.node_, future)
            return future.result()


    class OntoPublisher:
        def __init__(self, pub):
            self.pub: Publisher = pub

        def publish(self, msg):
            self.pub.publish(msg)


    class OntoSubscriber:
        def __init__(self, sub):
            self.sub: Subscription = sub


    class OntoROS(Node, metaclass=SingletonMeta):

        def __init__(self):
            super().__init__("OntoRos")

        @staticmethod
        def createService(srv_name, srv_type, connected) -> OntoService:
            return OntoService(OntoROS().create_client(srv_name, srv_type), OntoROS())

        @staticmethod
        def createPublisher(pub_name, pub_type, queue_size: int) -> OntoPublisher:
            return OntoPublisher(OntoROS().create_publisher(pub_type, pub_name, qos_profile=queue_size))

        @staticmethod
        def createSubscriber(sub_name, sub_type, callback, queue_size: int = 10) -> OntoSubscriber:
            return OntoSubscriber(OntoROS().create_subscription(sub_type, sub_name, callback, qos_profile=queue_size))

        @staticmethod
        def getRosTime() -> Time:
            return OntoROS().get_clock().now()

        @staticmethod
        def isShutdown() -> bool:
            return not rclpy.ok()
