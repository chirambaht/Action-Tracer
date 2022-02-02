using System.Collections;
using System.Collections.Generic;
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using UnityEngine;
using System.Globalization;

public class player : MonoBehaviour
{
    // Start is called before the first frame update
    static int NUMBER_OF_DEVICES = 3;
    static int DATA_POINTS = 4;

    bool jumpKeyPressed = false;
    public Transform groundCheckTransform;
    public GameObject bone_upper_right;
    public GameObject bone_lower_right;
    public GameObject bone_hand_right;

    float[,] vals = new float[NUMBER_OF_DEVICES, DATA_POINTS];

    float horizontalInput;
    float verticalInput;
    float jumpInput;

    float x_adj;
    float y_adj;
    float z_adj;
    float w_adj;

    float[] offsets = new float[4];
    Rigidbody mainPlayer;

    public const int port = 9022;
    UdpClient client;
    Thread networkThread;
    void Start()
    {
        x_adj = 0.0f;
        y_adj = 0.0f;
        z_adj = 0.0f;
        w_adj = 0.0f;

        mainPlayer = GetComponent<Rigidbody>();
        bone_upper_right = GameObject.Find("UpperArm.R");
        bone_lower_right = GameObject.Find("LowerArm.R");
        bone_hand_right = GameObject.Find("Hand.R");

        client = new UdpClient();
        Array.Clear(vals, 0, 2);

        networkThread = new Thread(new ThreadStart(GetNetData));
        networkThread.IsBackground = true;
        networkThread.Start();
    }




    void GetNetData()
    {
        IPEndPoint me = new IPEndPoint(IPAddress.Parse("192.168.0.149"), port);
        client = new UdpClient(me);
        client.Client.Blocking = false;
        client.Client.ReceiveTimeout = 1000;

        Debug.Log("Started network thread. Listening on: " + client.Client.LocalEndPoint.ToString());

        Byte[] sendBytes = Encoding.ASCII.GetBytes("Is anybody there");
        
        try{
            client.Send(sendBytes, sendBytes.Length,me);
            Debug.Log("Sent hello vibe");
        }
        catch ( Exception e ){
            Debug.Log(e.ToString());	
            
        }

        while (true)
        {
            try
            {
                // receive bytes
                IPEndPoint anyIP = new IPEndPoint(IPAddress.Any, 9022);
                byte[] data = client.Receive(ref anyIP);

                // encode UTF8-coded bytes to text format
                

                string text = Encoding.UTF8.GetString(data);

                // Debug.Log(text);

                
                string[] devices = text.Split(':');

                for (var dev = 0; dev < NUMBER_OF_DEVICES; dev++)
                {
                    string[] single_device = devices[dev].Split(',');


                    for (var val = 0; val < DATA_POINTS; val++)
                    {
                        vals[dev, val] = float.Parse(single_device[val], System.Globalization.CultureInfo.InvariantCulture);
                    }
                }


            }
            catch (Exception err)
            {
                err.ToString();
                // Debug.Log("Error");
            }
        }
    }


    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey(KeyCode.Space))
        {
            jumpKeyPressed = true;
        }

        jumpInput = Input.GetAxis("Jump");


        if ( Input.GetKey(KeyCode.X)){
            if (Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift)){
                x_adj += -0.01f;
            }
            else{
                x_adj += 0.01f;
            }
        }

        if ( Input.GetKey(KeyCode.Y)){
            if (Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift)){
                y_adj += -0.01f;
            }
            else{
                y_adj += 0.01f;
            }
        }

        if ( Input.GetKey(KeyCode.Z)){
            if (Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift)){
                z_adj += -0.01f;
            }
            else{
                z_adj += 0.01f;
            }
        }

        if ( Input.GetKey(KeyCode.W)){
            if (Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift)){
                w_adj += -0.01f;
            }
            else{
                w_adj += 0.01f;
            }
        }
        
        if ( Input.GetKey(KeyCode.O)){
            Debug.Log("Offsets for the work are as follows: ");
            Debug.Log(" X - " + x_adj.ToString());
            Debug.Log(" Y - " + y_adj.ToString());
            Debug.Log(" Z - " + z_adj.ToString());
            Debug.Log(" W - " + w_adj.ToString());
        }

        if ( Input.GetKeyDown(KeyCode.P)){
            Debug.Log("Current packet is: ");
            Debug.Log("Upper - " + vals[0,1].ToString() + " " + vals[0,2].ToString() + " " + vals[0,3].ToString() + " " + vals[0,0].ToString());
            Debug.Log("Lower - " + vals[1,1].ToString() + " " + vals[1,2].ToString() + " " + vals[1,3].ToString() + " " + vals[1,0].ToString());
            Debug.Log("Hand  - " + vals[2,1].ToString() + " " + vals[2,2].ToString() + " " + vals[2,3].ToString() + " " + vals[2,0].ToString());

        }

        bone_upper_right.transform.rotation = new Quaternion(vals[0, 1] + x_adj, vals[0, 2] + y_adj, vals[0, 3] + z_adj , vals[0, 0] + w_adj);
        bone_lower_right.transform.rotation = new Quaternion(vals[1, 1] + x_adj, vals[1, 2] + y_adj, vals[1, 3] + z_adj , vals[1, 0] + w_adj);
        bone_hand_right.transform.rotation = new Quaternion(vals[2, 1] + x_adj, vals[2, 2] + y_adj, vals[2, 3] + z_adj , vals[2, 0] + w_adj);

        
        // spine.rotation = new Quaternion(vals[1, 1], vals[1, 2], vals[1, 3], vals[1, 0]);


    }

    void FixedUpdate()
    {
        // mainPlayer.velocity = new Vector3(horizontalInput, mainPlayer.velocity.y, 0);
        if (Physics.OverlapSphere(groundCheckTransform.position, 0.1f).Length <= 1)
        {
            return;
        }
        if (jumpKeyPressed)
        {
            mainPlayer.AddForce(Vector3.up * 7, ForceMode.VelocityChange);
            jumpKeyPressed = false;
        }
    }

    private void onTriggerEnter(Collision other)
    {
        Debug.Log("Crash");
        if (other.gameObject.name == "coin")
        {
            Destroy(other.gameObject);

        }
    }


    void OnApplicationQuit()
{
    try
    {
        client.Close();
    }
    catch(Exception e)
    {
        Debug.Log(e.Message);
    }
    }

}

