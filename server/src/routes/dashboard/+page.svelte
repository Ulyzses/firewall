<script lang="ts">
  import { getModalStore } from '@skeletonlabs/skeleton';
  import type { ModalSettings, ModalComponent, ModalStore } from '@skeletonlabs/skeleton';
  import plug from '$lib/assets/plug.svg';
  const modalStore = getModalStore();

  const modalAddDevice: ModalSettings = {
    backdropClasses: '!backdrop-blur-md',
    modalClasses: '!bg-white',
    type: 'prompt',
    // Data
    title: 'Enter Device ID',
    body: 'Provide the MAC address of your device below, e.g. 34:38:3a:45:37:3a',
    // Populates the input value and attributes
    value: '',
    valueAttr: { type: 'text', minlength: 17, maxlength: 17, required: true },
    // Returns the updated response value
    response: (r: string) => {
      addDeviceValue = r;
      addDevice();
    }
  };
  
  let addDeviceValue = '';

  const statusColors = ['bg-firewall-red', 'bg-green-500'];
  const statusLabels = ['OFF', 'ON'];
              
  interface Device {
    name: string;
    mac: string;
    status: boolean;
    smoke: boolean;
  }

  export let data;

  $: ({ user, supabase, devices } = data);

  let addDeviceForm: HTMLFormElement;

  async function addDevice() {
    const { data, error } = await supabase
      .from('devices')
      .insert({
        user: user?.id,
        mac: addDeviceValue
      })
      .select();

    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }
  }

  async function toggleDevice(device: Device) {
    const { data, error } = await supabase
      .from('actions')
      .insert({
        mac: device.mac,
        action: device.status ? 'OFF' : 'ON'
      })
      .select();
    
    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }

    device.status = !device.status;
    devices = devices;
  }
</script>

<div class='grid grid-flow-row grid-cols-1 lg:grid-cols-2 gap-4 mx-6'>
  <div class='flex flex-row justify-between w-full h-min lg:col-span-2 items-center'>
    <h1 class='text-3xl font-kollektif'>Devices</h1>
    <label for="macAddress">
      <input name="macAddress" type="hidden" bind:value={ addDeviceValue }/>
    </label>
    <button class='border border-white bg-firewall-red p-4 rounded-lg w-40 h-min text-sm text-white font-DM hover:border-firewall-red' on:click={() => { modalStore.trigger(modalAddDevice); }}>+ Add Device</button>
  </div>
  <div class='overflow-y-scroll'>
    {#each devices as device (device.id)}
      <div class='flex flex-row flex-none w-auto h-40 p-8 items-center bg-gray-200 rounded-lg border border-gray-200 font-DM gap-6 justify-between font-bold'>
        <img src={ plug } alt='' class='size-28 md:size-32' />
        <div class='basis-1/3 h-auto w-1/4 grid grid-flow-cols justify-items-left justify-self-center'>
          <span>{ device.name }</span>
          <span class='italic'>{ device.mac }</span>
          <span class='text-firewall-red text-sm'>         
            {#if device.smoke}
              Smoke Detected!
            {:else}
              <br>
            {/if}
          </span>
        </div>
        <button class='h-full w-1/4 { statusColors[Number(device.status)] } text-white rounded-lg text-center border border-gray-200 hover:border-0' on:click={() => toggleDevice(device)}>
          <span>STATUS:</span>
          <br>
          <span class='font-bold'>{ statusLabels[Number(device.status)] }</span>
        </button>
      </div>
    {/each}
  </div>
</div>
